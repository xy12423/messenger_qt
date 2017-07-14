#include "stdafx.h"
#include <QFileInfo>
#ifdef ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif
#include "crypto.h"
#include "main.h"

QString TEMP_PATH, DATA_PATH, DOWNLOAD_PATH;
const QString EmptyQString;

static const char* privatekeyFile = ".privatekey";

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
            JPG_HEADER[] = "\xFF\xD8\xFF\x00",
            GIF_HEADER[] = "\x47\x49\x46";
    if (size >= sizeof(BMP_HEADER) && compare_header(data, BMP_HEADER))
        return true;
    if (size >= sizeof(PNG_HEADER) && compare_header(data, PNG_HEADER))
        return true;
    if (size >= sizeof(JPG_HEADER) && compare_header(data, JPG_HEADER))
    {
        switch (data[3])
        {
            case '\xE0':
            case '\xE1':
            case '\xE2':
            case '\xE3':
            case '\xE8':
            case '\xEB':
            case '\xED':
                return true;
        }
    }
    if (size >= sizeof(GIF_HEADER) && compare_header(data, GIF_HEADER))
        return true;
    return false;
}

QtWindowInterface::QtWindowInterface(QGuiApplication& app)
    :file_block(std::make_unique<char[]>(FileBlockLen))
{
    QDir fs;
    TEMP_PATH = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    DATA_PATH = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    DOWNLOAD_PATH = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    if (!fs.exists(DATA_PATH))
        fs.mkpath(DATA_PATH);
    QDir::setCurrent(DATA_PATH);

    fs.cd(DATA_PATH);
    cryp_helper = std::make_unique<crypto::provider>(fs.filePath(privatekeyFile).toLocal8Bit().data(), true);

    port_type portListen = 4826;
    port_type portsBegin = 5000, portsEnd = 9999;
    bool use_v6 = false;
    int crypto_worker = 1;

    crypto_srv = std::make_unique<crypto::server>(threadNetwork.get_io_service(), crypto_worker);
    srv = std::make_unique<qt_srv_interface>(threadNetwork.get_io_service(), threadMisc.get_io_service(),
        asio::ip::tcp::endpoint((use_v6 ? asio::ip::tcp::v6() : asio::ip::tcp::v4()), portListen),
        *crypto_srv.get(),
        this, *cryp_helper.get());

    std::srand(static_cast<unsigned int>(std::time(NULL)));
    for (; portsBegin <= portsEnd; portsBegin++)
        srv->free_rand_port(portsBegin);

    srv->start();

    threadMisc.start();
    threadNetwork.start();

    qRegisterMetaType<std::shared_ptr<std::function<void()>>>("std::shared_ptr<std::function<void()>>");
    connect(this, SIGNAL(selectIndex(int)), this, SLOT(onSelectChanged(int)));
    connect(this, SIGNAL(executeFunc(const std::shared_ptr<std::function<void()>>&)), this, SLOT(onExecuteFunc(const std::shared_ptr<std::function<void()>>&)));
    connect(&app, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));

    LoadConnHistory();
}

QtWindowInterface::~QtWindowInterface()
{
    srv->shutdown();
    crypto_srv->stop();

    threadNetwork.stop();
    threadMisc.stop();
    while (!threadNetwork.is_stopped() || !threadMisc.is_stopped());

    srv.reset();
    crypto_srv.reset();

    SaveConnHistory();
}

void QtWindowInterface::RecvMsg(user_id_type id, const std::string& msg, const std::string& from)
{
    ExecuteHandler([this, id, msg, from]() {
        user_ext_type &usr = user_ext.at(id);
        int msgID = static_cast<int>(usr.log.size());
        bool has_from = !from.empty();
        if (has_from)
            usr.log.emplace_back(from.c_str(), msg);
        else
            usr.log.emplace_back(usr.addr, msg);
        if (selected != -1 && id == user_id_map.at(selected))
        {
            if (has_from)
                emit chatText(msgID, QString::fromUtf8(from.c_str()), QString::fromUtf8(msg.c_str()));
            else
                emit chatText(msgID, usr.addr, QString::fromUtf8(msg.c_str()));
        }
    });
    if (is_in_background)
        notify("Messenger", "New Message!");
}

void QtWindowInterface::RecvImg(user_id_type id, const QString& path, const std::string& from)
{
    ExecuteHandler([this, id, path, from]() {
        user_ext_type &usr = user_ext.at(id);
        int msgID = static_cast<int>(usr.log.size());
        bool has_from = !from.empty();
        if (!from.empty())
            usr.log.emplace_back(from.c_str(), path, true);
        else
            usr.log.emplace_back(usr.addr, path, true);
        if (selected != -1 && id == user_id_map.at(selected))
        {
            if (has_from)
                emit chatImage(msgID, QString::fromUtf8(from.c_str()), path);
            else
                emit chatImage(msgID, usr.addr, path);
        }
    });
    if (is_in_background)
        notify("Messenger", "New Message!");
}

void QtWindowInterface::RecvFileH(user_id_type id, const QString& file_path, size_t block_count)
{
    userFileTransferInfo &trans = userFileTrans.at(id);
    trans.recvFile = file_path;
    trans.blockAll = static_cast<int>(block_count);
    trans.blockLast = static_cast<int>(block_count);

    int fileID = static_cast<int>(fileProgress.size());
    trans.recvID = fileID;
    fileProgress.push_back(0);

    ExecuteHandler([this, id, fileID, file_path]() {
        user_ext_type &usr = user_ext.at(id);
        usr.log.emplace_back(usr.addr, file_path, fileID);
        if (selected != -1 && id == user_id_map.at(selected))
            emit chatFile(fileID, usr.addr, file_path);
    });
}

void QtWindowInterface::RecvFileB(user_id_type id, const char* data, size_t size)
{
    userFileTransferInfo &trans = userFileTrans.at(id);

    if (trans.blockLast > 0)
    {
        std::ofstream fout(trans.recvFile.toLocal8Bit().data(), std::ios::out | std::ios::binary | std::ios::app);
        fout.write(data, size);
        fout.close();
        trans.blockLast--;

        try
        {
            int progress = (trans.blockAll - trans.blockLast) * 100 / trans.blockAll;
            fileProgress.at(trans.recvID) = progress;
            ExecuteHandler([this, id, recvID = trans.recvID, progress]() {
                if (selected != -1 && id == user_id_map.at(selected))
                    emit notifyFileProgress(recvID, progress);
            });
        }
        catch (...) {}

        if (trans.blockLast == 0)
        {
            trans.recvFile.clear();
            trans.recvID = -1;
            trans.blockAll = 0;
        }
    }
}

void QtWindowInterface::RecvFeature(user_id_type id, int flag)
{
    ExecuteHandler([this, id, flag]() {
        user_ext_type &usr = user_ext.at(id);
        usr.feature = flag;
        if (selected != -1 && id == user_id_map.at(selected))
            emit enableFeature(flag);
    });
}

void QtWindowInterface::RecvFileList(user_id_type id, std::vector<std::pair<std::string, std::string>>& _list)
{
    ExecuteHandler([this, id, list = std::move(_list)]() {
        user_ext_type &usr = user_ext.at(id);
        if ((usr.feature & feature_file_storage) == 0)
            return;
        usr.file_list = std::move(list);
        if (selected != -1 && id == user_id_map.at(selected))
            emit refreshFilelist(GenerateFilelist(usr));
    });
}

void QtWindowInterface::Join(user_id_type id, const std::string& key)
{
    ExecuteHandler([this, id, key]() {
        user_ext_type &usr = user_ext[id];
        usr.addr = srv->get_session(id).get_address().c_str();
        usr.key = key;

        QString name = usr.addr;
        try
        {
            usr.comment = srv->get_key_ex(key);
            usr.have_comment = true;
            name.append('(');
            name.append(QString::fromStdString(usr.comment));
            name.append(')');
        }
        catch (std::out_of_range&) {}
        int new_index = static_cast<int>(user_id_map.size());
        user_id_map.push_back(id);
        emit joined(new_index, name);
        if (selected == -1)
        {
            selected = new_index;
            emit selectIndex(selected);
        }
    });
    userFileTrans.emplace(id, userFileTransferInfo());
}

void QtWindowInterface::Leave(user_id_type id)
{
    ExecuteHandler([this, id]() {
        int i = 0;
        std::vector<user_id_type>::iterator itr = user_id_map.begin(), itrEnd = user_id_map.end();
        for (; itr != itrEnd && *itr != id; itr++) i++;
        if (selected == i)
        {
            selected = -1;
            emit selectIndex(selected);
        }
        emit left(i);
        if (selected > i)
            selected -= 1;
        user_id_map.erase(itr);
        user_ext.erase(id);
    });
    userFileTrans.erase(id);
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
        AddConnHistory(addr.trimmed(), static_cast<port_type>(port));
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
            user_ext_type &usr = user_ext.at(uID);
            int msgID = static_cast<int>(usr.log.size());
            srv->send_data(uID, std::move(msg_utf8), msgr_proto::session::priority_msg);
            usr.log.emplace_back("Me", msg);
            emit chatText(msgID, QString("Me"), msg);
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
            while (fin.good())
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

            user_ext_type &usr = user_ext.at(uID);
            int msgID = static_cast<int>(usr.log.size());
            srv->send_data(uID, std::move(data), msgr_proto::session::priority_msg);
            usr.log.emplace_back("Me", new_path, true);
            emit chatImage(msgID, QString("Me"), new_path);
        }
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::sendFile(const QUrl& url)
{
    try
    {
        user_id_type uID = user_id_map.at(selected);
        QString path = url.toLocalFile();

        ExecuteHandlerMisc([this, uID, path]() {
            QFileInfo file_info(path);
            if (!path.isEmpty() && file_info.exists())
            {
                std::string file_name = path.toUtf8().toStdString();
                size_t pos = file_name.rfind('/');
                if (pos != std::string::npos)
                    file_name.erase(0, pos + 1);
                pos = file_name.rfind('\\');
                if (pos != std::string::npos)
                    file_name.erase(0, pos + 1);

                qint64 file_size = file_info.size();
                data_size_type blockCountAll;
                if (file_size % FileBlockLen == 0)
                    blockCountAll = file_size / FileBlockLen;
                else
                    blockCountAll = file_size / FileBlockLen + 1;
                if (blockCountAll < 1)
                    return;
                size_t file_name_size = file_name.size();
                std::string data(9, '\0');

                data[0] = PAC_TYPE_FILE_H;
                data[1] = static_cast<uint8_t>(blockCountAll & 0xFF);
                data[2] = static_cast<uint8_t>(blockCountAll >> 8);
                data[3] = static_cast<uint8_t>(blockCountAll >> 16);
                data[4] = static_cast<uint8_t>(blockCountAll >> 24);
                data[5] = static_cast<uint8_t>(file_name_size & 0xFF);
                data[6] = static_cast<uint8_t>(file_name_size >> 8);
                data[7] = static_cast<uint8_t>(file_name_size >> 16);
                data[8] = static_cast<uint8_t>(file_name_size >> 24);
                data.append(file_name);

                userFileTransferInfo &trans = userFileTrans.at(uID);
                bool sending = !trans.sendTasks.empty();

                int fileID = static_cast<int>(fileProgress.size());
                trans.sendTasks.emplace_back(data, path, blockCountAll, fileID);
                fileProgress.push_back(0);
                if (!sending)
                    SendFileBlock(uID);

                ExecuteHandler([this, uID, path, fileID]() {
                    user_ext_type &usr = user_ext.at(uID);
                    usr.log.emplace_back("Me", path, fileID);
                    emit chatFile(fileID, "Me", path);
                });
            }
        });
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::reqFileProgress(int id)
{
    try
    {
        ExecuteHandlerMisc([this, id]() {
            emit notifyFileProgress(id, fileProgress.at(id));
        });
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::reqConnHistory()
{
    try
    {
        QStringList addr, port;
        for (auto itr = connHistory.rbegin(), itrEnd = connHistory.rend(); itr != itrEnd; itr++)
        {
            addr.push_back(itr->first);
            port.push_back(QString::number(itr->second));
        }
        emit refreshConnHistory(addr, port);
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::reqConnHistoryDel(int index)
{
    try
    {
        connHistory.erase(connHistory.end() - index - 1);
        emit reqConnHistory();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::reqConnHistoryClr()
{
    try
    {
        connHistory.clear();
        emit reqConnHistory();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}


void QtWindowInterface::reqFilelist()
{
    try
    {
        user_id_type uID = user_id_map.at(selected);
        user_ext_type &usr = user_ext.at(uID);
        if ((usr.feature & feature_file_storage) == 0)
            return;
        emit refreshFilelist(GenerateFilelist(usr));

        std::string data;
        data.push_back(PAC_TYPE_PLUGIN_DATA);
        data.push_back(pak_file_storage);
        data.push_back(0);
        srv->send_data(uID, data, msgr_proto::session::priority_plugin);
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::reqDownloadFile(int index)
{
    try
    {
        user_id_type uID = user_id_map.at(selected);
        user_ext_type &usr = user_ext.at(uID);
        if ((usr.feature & feature_file_storage) == 0)
            return;

        std::string data;
        data.push_back(PAC_TYPE_PLUGIN_DATA);
        data.push_back(pak_file_storage);
        data.push_back(1);
        data.append(file_id_map.at(index));
        srv->send_data(uID, data, msgr_proto::session::priority_plugin);
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::reqKeylist()
{
    try
    {
        GenerateKeylist();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::importKey(const QUrl& key_path)
{
    try
    {
        QDir fs;
        QString path = key_path.toLocalFile();
        if (!path.isEmpty() && fs.exists(path))
        {
            std::ifstream fin(path.toLocal8Bit().data(), std::ios_base::in | std::ios_base::binary);
            srv->import_key(fin);
            fin.close();
        }
        GenerateKeylist();
        RefreshComments();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::exportKey(int index, const QUrl& key_path, const QString& file_name)
{
    try
    {
        std::string &key = key_list.at(index).key;
        QDir fs(key_path.toLocalFile());
        QString path = fs.filePath(file_name);
        if (!path.isEmpty())
        {
            std::ofstream fout(path.toLocal8Bit().data(), std::ios_base::out | std::ios_base::binary);
            srv->export_key(fout, key);
            fout.close();
        }
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::modifyKey(int index, const QString& ex)
{
    try
    {
        key_item &key = key_list.at(index);
        key.ex = ex.toStdString();
        srv->edit_key(key.key, key.ex);
        RefreshComments();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::trustKey()
{
    try
    {
        if (selected != -1)
        {
            const std::string &key = srv->get_session(user_id_map.at(selected)).get_key();
            srv->certify_key(key, "");
            GenerateKeylist();
            RefreshComments();
        }
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::distrustKey(int index)
{
    try
    {
        std::string &key = key_list.at(index).key;
        srv->decertify_key(key);
        GenerateKeylist();
        RefreshComments();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::onApplicationStateChanged(Qt::ApplicationState state)
{
    try
    {
        bool is_in_background_now = ((state & (Qt::ApplicationInactive | Qt::ApplicationActive)) == 0);
        if (is_in_background && !is_in_background_now)
        {
            ExecuteHandlerMisc([this]() {
                notifyClear();
            });
        }
        is_in_background = is_in_background_now;
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::onSelectChanged(int index)
{
    try
    {
        if (index != -1)
        {
            user_ext_type &usr = user_ext.at(user_id_map.at(index));
            emit enableFeature(usr.feature);
            emit chatClear();
            std::vector<user_ext_type::log_item>::const_iterator itr, itr_end;
            int msgID = 0;
            for (itr = usr.log.cbegin(), itr_end = usr.log.cend(); itr != itr_end; itr++)
            {
                switch (itr->type)
                {
                case user_ext_type::log_item::ITEM_TEXT:
                    emit chatText(msgID, itr->from, itr->msg);
                    break;
                case user_ext_type::log_item::ITEM_IMAGE:
                    emit chatImage(msgID, itr->from, itr->image);
                    break;
                case user_ext_type::log_item::ITEM_FILE:
                    emit chatFile(itr->fileID, itr->from, itr->fileName);
                    break;
                }
                msgID += 1;
            }
        }
        else
        {
            emit enableFeature(0);
            emit chatClear();
            emit refreshFilelist(GenerateFilelist());
        }
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::onExecuteFunc(const std::shared_ptr<std::function<void ()> > &funcPtr)
{
    try
    {
        (*funcPtr)();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::SendFileBlock(int uID)
{
    try
    {
        if (uID < 0 || uID > std::numeric_limits<user_id_type>::max() || user_ext.count(static_cast<user_id_type>(uID)) < 1)
            return;
        std::list<userFileTransferInfo::sendTask> &tasks = userFileTrans.at(static_cast<user_id_type>(uID)).sendTasks;
        if (tasks.empty())
            return;
        userFileTransferInfo::sendTask &task = tasks.front();
        if (task.blockCount == 0)
            srv->send_data(static_cast<user_id_type>(uID), std::move(task.header), msgr_proto::session::priority_file);

        std::string sendBuf;
        task.fin.read(file_block.get(), FileBlockLen);
        std::streamsize sizeRead = task.fin.gcount();
        sendBuf.reserve(5 + sizeRead);
        sendBuf.push_back(PAC_TYPE_FILE_B);
        sendBuf.push_back(static_cast<uint8_t>(sizeRead & 0xFF));
        sendBuf.push_back(static_cast<uint8_t>(sizeRead >> 8));
        sendBuf.push_back(static_cast<uint8_t>(sizeRead >> 16));
        sendBuf.push_back(static_cast<uint8_t>(sizeRead >> 24));
        sendBuf.append(file_block.get(), sizeRead);

        task.blockCount += 1;
        int progress = task.blockCount * 100 / task.blockCountAll;
        try
        {
            fileProgress.at(task.sendID) = progress;
        }
        catch (...) {}

        srv->send_data(static_cast<user_id_type>(uID), std::move(sendBuf), msgr_proto::session::priority_file, [this, uID, tID = task.sendID, progress]() {
            ExecuteHandler([this, uID, tID, progress](){
                if (selected != -1 && uID == user_id_map.at(selected))
                    emit notifyFileProgress(tID, progress);
            });
            ExecuteHandlerMisc([this, uID]() {
                SendFileBlock(uID);
            });
        });

        if (task.fin.eof() || task.blockCount >= task.blockCountAll)
            tasks.pop_front();
    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

QStringList QtWindowInterface::GenerateFilelist()
{
    file_id_map.clear();
    return QStringList();
}

QStringList QtWindowInterface::GenerateFilelist(user_ext_type &usr)
{
    QStringList name_list;
    file_id_map.clear();

    for (auto itr = usr.file_list.begin(), itr_end = usr.file_list.end(); itr != itr_end; itr++)
    {
        name_list.append(QString::fromStdString(itr->second));
        file_id_map.push_back(itr->first);
    }

    return name_list;
}

void QtWindowInterface::GenerateKeylist()
{
    key_list.clear();
    srv->list_key(key_list);
    QStringList list_key, list_ex;
    for (auto itr = key_list.cbegin(), itr_end = key_list.cend(); itr != itr_end; itr++)
    {
        std::string str;
        crypto::provider::hash_short(itr->key, str);
        list_key.append(QString::fromStdString(str));
        list_ex.append(QString::fromStdString(itr->ex));
    }
    emit refreshKeylist(list_key, list_ex);
}

void QtWindowInterface::RefreshComments()
{
    for (int i = 0; i < static_cast<int>(user_id_map.size()); i++)
    {
        user_ext_type &usr = user_ext.at(user_id_map[i]);
        try
        {
            const std::string &comment = srv->get_key_ex(usr.key);
            if (!usr.have_comment || usr.comment != comment)    //Add or Modify
            {
                usr.comment = comment;
                QString new_name = usr.addr;
                new_name.append('(');
                new_name.append(QString::fromStdString(comment));
                new_name.append(')');
                emit changeName(i, new_name);
            }
            usr.have_comment = true;
        }
        catch (std::out_of_range&)
        {
            if (usr.have_comment)   //Remove
                emit changeName(i, usr.addr);
            usr.comment.clear();
            usr.have_comment = false;
        }
    }
}

void QtWindowInterface::LoadConnHistory()
{
    std::ifstream fin(".connhistory");
    char size_buf[sizeof(uint16_t)], port_buf[sizeof(port_type)];
    std::vector<char> buf;

    fin.read(size_buf, sizeof(uint16_t));
    while (fin.good())
    {
        //read addr
        buf.resize(u16_from_data(size_buf));
        fin.read(buf.data(), buf.size());
        if (fin.fail())
            break;
        //read port
        fin.read(port_buf, sizeof(port_type));
        //emplace
        connHistory.emplace_back(QString::fromUtf8(buf.data(), buf.size()), u16_from_data(port_buf));
        //read next size
        fin.read(size_buf, sizeof(uint16_t));
    }

    fin.close();
}

void QtWindowInterface::SaveConnHistory()
{
    std::ofstream fout(".connhistory");

    for (auto itr = connHistory.begin(), itrEnd = connHistory.end(); itr != itrEnd; itr++)
    {
        std::string addr = itr->first.toStdString();
        port_type port = itr->second;
        fout.put(static_cast<char>(addr.size()));
        fout.put(static_cast<char>(addr.size() >> 8));
        fout.write(addr.data(), addr.size());
        fout.put(static_cast<char>(port));
        fout.put(static_cast<char>(port >> 8));
    }

    fout.close();
}

void QtWindowInterface::AddConnHistory(const QString& addr, port_type port)
{
    for (auto itr = connHistory.begin(), itrEnd = connHistory.end(); itr != itrEnd; itr++)
    {
        if (itr->first == addr && itr->second == port)
        {
            connHistory.erase(itr);
            break;
        }
    }
    connHistory.emplace_back(addr, port);
}

#ifdef ANDROID
void QtWindowInterface::notifyMessage(const QString& title, const QString& msg)
{
    try
    {
        QAndroidJniObject title_java = QAndroidJniObject::fromString(title),
                          msg_java   = QAndroidJniObject::fromString(msg);
        QAndroidJniObject::callStaticMethod<void>("org/xy12423/messenger_qt/QtJavaInter",
                                                  "notify",
                                                  "(Ljava/lang/String;Ljava/lang/String;)V",
                                                  title_java.object<jstring>(),
                                                  msg_java.object<jstring>());

    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}

void QtWindowInterface::notifyMessageClear()
{
    try
    {
        QAndroidJniObject::callStaticMethod<void>("org/xy12423/messenger_qt/QtJavaInter",
                                                  "notifyCancel",
                                                  "()V");

    }
    catch (std::exception& ex)
    {
        srv->on_exception(ex);
    }
}
#endif
