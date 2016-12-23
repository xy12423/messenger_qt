#include "stdafx.h"
#include <QStandardPaths>
#include "crypto.h"
#include "main.h"

QString TEMP_PATH, DATA_PATH, DOWNLOAD_PATH;
const QString EmptyQString;

const char* privatekeyFile = ".privatekey";

bool compare_header(const char* data, const char* header)
{
    for (; *header != '\0'; ++header, ++data)
        if (*data != *header)
            return false;
    return true;
}

bool is_image(const char* data, size_t size)
{
    const char BMP_HEADER[] = "\x42\x4D",
            PNG_HEADER[] = "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A",
            JPG_HEADER[] = "\xFF\xD8\xFF\xE0",
            GIF_HEADER[] = "\x47\x49\x46";
    if (size >= sizeof(BMP_HEADER) && compare_header(data, BMP_HEADER))
        return true;
    if (size >= sizeof(PNG_HEADER) && compare_header(data, PNG_HEADER))
        return true;
    if (size >= sizeof(JPG_HEADER) && compare_header(data, JPG_HEADER))
        return true;
    if (size >= sizeof(GIF_HEADER) && compare_header(data, GIF_HEADER))
        return true;
    return false;
}

QtWindowInterface::QtWindowInterface()
{
    QDir fs;
    TEMP_PATH = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    DATA_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    DOWNLOAD_PATH = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    qDebug(TEMP_PATH.toUtf8().data());
    qDebug(DATA_PATH.toUtf8().data());
    qDebug(DOWNLOAD_PATH.toUtf8().data());

    if (!fs.exists(DATA_PATH))
        fs.mkpath(DATA_PATH);
    QDir::setCurrent(DATA_PATH);

    fs.cd(DATA_PATH);
    cryp_helper = std::make_unique<ECC_crypto_helper>(fs.filePath(privatekeyFile).toLocal8Bit().data());

    port_type portListen = 4826;
    port_type portsBegin = 5000, portsEnd = 9999;
    bool use_v6 = false;
    int crypto_worker = 1;

    crypto_srv = std::make_unique<crypto::server>(threadCrypto.get_io_service(), crypto_worker);
    srv = std::make_unique<qt_srv_interface>(threadNetwork.get_io_service(), threadMisc.get_io_service(),
        asio::ip::tcp::endpoint((use_v6 ? asio::ip::tcp::v6() : asio::ip::tcp::v4()), portListen),
        *crypto_srv.get(),
        this, *cryp_helper.get());

    std::srand(static_cast<unsigned int>(std::time(NULL)));
    for (; portsBegin <= portsEnd; portsBegin++)
        srv->free_rand_port(portsBegin);

    srv->start();

    threadCrypto.start();
    threadMisc.start();
    threadNetwork.start();

    connect(this, SIGNAL(selectIndex(int)), this, SLOT(OnSelectChanged(int)));
}

QtWindowInterface::~QtWindowInterface()
{
    srv->shutdown();
    crypto_srv->stop();

    threadCrypto.stop();
    threadNetwork.stop();
    threadMisc.stop();
    while (!threadCrypto.is_stopped() || !threadNetwork.is_stopped() || !threadMisc.is_stopped());

    srv.reset();
    crypto_srv.reset();
}

void QtWindowInterface::RecvMsg(user_id_type id, const std::string& msg)
{
    user_ext_type &usr = user_ext.at(id);
    QString msg_q(msg.data());
    usr.log.emplace_back(true, msg_q);
    if (selected != -1 && id == user_id_map.at(selected))
        emit refreshChat(GenerateLogStr(usr));
}

void QtWindowInterface::RecvImg(user_id_type id, const QString& path)
{
    user_ext_type &usr = user_ext.at(id);
    usr.log.emplace_back(true, path, true);
    if (selected != -1 && id == user_id_map.at(selected))
        emit refreshChat(GenerateLogStr(usr));
}

void QtWindowInterface::RecvFileH(user_id_type id, const QString& file_name, size_t block_count)
{
    user_ext_type &usr = user_ext.at(id);
    usr.recvFile = file_name;
    usr.blockLast = static_cast<int>(block_count);
}

void QtWindowInterface::RecvFileB(user_id_type id, const char* data, size_t size)
{
    user_ext_type &usr = user_ext.at(id);

    if (usr.blockLast > 0)
    {
        std::ofstream fout(usr.recvFile.toLocal8Bit().data(), std::ios::out | std::ios::binary | std::ios::app);
        fout.write(data, size);
        fout.close();
        usr.blockLast--;

        if (usr.blockLast == 0)
            usr.recvFile.clear();
    }
}

void QtWindowInterface::Join(user_id_type id, const std::string&)
{
    user_ext_type &ext = user_ext[id];
    ext.addr = srv->get_session(id).get_address().c_str();

    QString &name = ext.addr;
    int new_index = static_cast<int>(user_id_map.size());
    user_id_map.push_back(id);
    emit joined(new_index, name);
    if (selected == -1)
    {
        selected = new_index;
        emit selectIndex(selected);
    }
}

void QtWindowInterface::Leave(user_id_type id)
{
    int i = 0;
    std::vector<user_id_type>::iterator itr = user_id_map.begin(), itrEnd = user_id_map.end();
    for (; itr != itrEnd && *itr != id; itr++) i++;
    if (selected == i)
    {
        selected = -1;
        emit selectIndex(selected);
    }
    emit left(i);
    user_id_map.erase(itr);
    user_ext.erase(id);
}

void QtWindowInterface::connectTo(const QString& addr, const QString& port_str)
{
    try
    {
        bool is_int;
        int port = port_str.toInt(&is_int);
        if (!is_int)
            throw(std::out_of_range("Port is not a number"));
        if (port < std::numeric_limits<port_type>::min() || port > std::numeric_limits<port_type>::max())
            throw(std::out_of_range("Port out of range"));
        srv->connect(addr.toStdString(), static_cast<port_type>(port));
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::disconnect()
{
    try
    {
        srv->disconnect(user_id_map.at(selected));
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::sendMsg(const QString& msg)
{
    try
    {
        if (!msg.isEmpty() && selected != -1)
        {
            QByteArray msg_buf = msg.toUtf8();
            char tmp[5];
            tmp[0] = PAC_TYPE_MSG;
            tmp[1] = static_cast<uint8_t>(msg_buf.size() & 0xFF);
            tmp[2] = static_cast<uint8_t>(msg_buf.size() >> 8);
            tmp[3] = static_cast<uint8_t>(msg_buf.size() >> 16);
            tmp[4] = static_cast<uint8_t>(msg_buf.size() >> 24);
            std::string msg_utf8(tmp, sizeof(tmp));
            msg_utf8.append(msg_buf.data());

            user_id_type uID = user_id_map.at(selected);
            srv->send_data(uID, msg_utf8, msgr_proto::session::priority_msg);
            user_ext.at(uID).log.emplace_back(false, msg);
            emit refreshChat(GenerateLogStr(user_ext.at(uID)));
        }
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::sendImg(const QUrl& url)
{
    try
    {
        QDir fs;
        QString path = url.toLocalFile();
        if (!path.isEmpty() && fs.exists(path))
        {
            user_id_type uID = user_id_map.at(selected);
            int next_image_id;
            srv->new_image_id(next_image_id);

            QDir tmp_path = TEMP_PATH;
            tmp_path.cd(IMG_TMP_PATH_NAME);
            tmp_path.cd(QString::number(uID));
            QString new_path = tmp_path.filePath(".messenger_tmp_" + QString::number(next_image_id));

            std::ifstream fin(path.toLocal8Bit().data(), std::ios_base::in | std::ios_base::binary);
            std::ofstream fout(new_path.toLocal8Bit().data(), std::ios_base::out | std::ios_base::binary);
            size_t file_size = 0;
            if (!fin || !fin.is_open() || !fout || !fout.is_open())
                return;

            constexpr size_t buf_size = 0x100000;
            std::unique_ptr<char[]> buf = std::make_unique<char[]>(buf_size);
            std::string data(5, '\0');

            fin.read(buf.get(), buf_size);
            if (!is_image(buf.get(), fin.gcount()))
                return;
            file_size += fin.gcount();
            fout.write(buf.get(), fin.gcount());
            data.append(buf.get(), fin.gcount());
            while (!fin.eof())
            {
                fin.read(buf.get(), buf_size);
                file_size += fin.gcount();
                fout.write(buf.get(), fin.gcount());
                data.append(buf.get(), fin.gcount());
            }

            fout.close();
            fin.close();

            data[0] = PAC_TYPE_IMAGE;
            data[1] = static_cast<uint8_t>(file_size & 0xFF);
            data[2] = static_cast<uint8_t>(file_size >> 8);
            data[3] = static_cast<uint8_t>(file_size >> 16);
            data[4] = static_cast<uint8_t>(file_size >> 24);

            srv->send_data(uID, data, msgr_proto::session::priority_msg);
            user_ext.at(uID).log.emplace_back(false, new_path, true);
            emit refreshChat(GenerateLogStr(user_ext.at(uID)));
        }
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::OnSelectChanged(int index)
{
    try
    {
        if (index != -1)
            emit refreshChat(GenerateLogStr(user_ext.at(user_id_map.at(index))));
        else
            emit refreshChat(EmptyQString);
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

QString QtWindowInterface::GenerateLogStr(user_ext_type &usr)
{
    QString ret;
    auto &log = usr.log;
    ret.append("<style type=\"text/css\"> pre { margin-top: 0px 0; margin-bottom: 0px 0; } b { margin-top: 0px 0; margin-bottom: 0px 0; } </style>");
    for (auto itr = log.begin(), itr_end = log.end(); itr != itr_end; itr++)
    {
        ret.append("<b>");
        if (itr->is_recv)
            ret.append(usr.addr);
        else
            ret.append("Me");
        ret.append("</b><pre>\n</pre>");
        if (itr->is_image)
        {
            ret.append("<img src=\"file:/");
            ret.append(itr->image);
            ret.append("\" />");
        }
        else
        {
            ret.append("<pre>");
            for (auto itr_c = itr->msg.begin(), itr_c_end = itr->msg.end(); itr_c != itr_c_end; itr_c++)
            {
                if (*itr_c == '<')
                    ret.append("&lt;");
                else if (*itr_c == '>')
                    ret.append("&gt;");
                else if (*itr_c == '&')
                    ret.append("&amp;");
                else
                    ret.push_back(*itr_c);
            }
            ret.append("</pre>");
        }
        ret.append("<pre>\n</pre>");
    }
    return ret;
}
