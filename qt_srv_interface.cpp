#include "stdafx.h"
#include "main.h"

//extern FileSendThread *threadFileSend;
extern QString TEMP_PATH, DATA_PATH, DOWNLOAD_PATH;
const char* IMG_TMP_PATH_NAME = ".messenger_tmp";
const char* IMG_TMP_FILE_NAME = ".messenger_tmp_";
QDir IMG_TMP_PATH;

const char* publickeysFile = ".publickey";

struct data_view
{
    data_view(const char* _data, size_t _size)
        :data(_data), size(_size)
    {}
    data_view(const std::string &_data)
        :data(_data.data()), size(_data.size())
    {}

    template <typename _Ty>
    void read(_Ty& ret);
    inline void read(char& ret) { if (size < 1) throw(qt_srv_interface_error()); ret = *data; data += 1; size -= 1; }
    inline void read(char* dst, size_t _size) { if (size < _size) throw(qt_srv_interface_error()); memcpy(dst, data, _size); data += _size; size -= _size; }
    inline void read(std::string& dst, size_t _size) { if (size < _size) throw(qt_srv_interface_error()); dst.append(data, _size); data += _size; size -= _size; }
    inline void check(size_t count) const { if (size < count) throw(qt_srv_interface_error()); }
    inline bool check_bool(size_t count) const { if (size < count) return false; return true; }
    inline void skip(size_t count) { if (size < count) throw(qt_srv_interface_error()); data += count; size -= count; }

    const char* data;
    size_t size;
};

template <typename _Ty>
void data_view::read(_Ty& ret)
{
    if (size < sizeof(_Ty))
        throw(qt_srv_interface_error());
    size -= sizeof(_Ty);

    const char *data_end = data + sizeof(_Ty);
    ret = 0;
    for (int i = 0; data < data_end; data++, i += 8)
        ret |= static_cast<uint64_t>(static_cast<uint8_t>(*data)) << i;
}

/*
bool compare_header(const data_view& data, const char* header)
{
    for (const char* data_itr = data.data; *header != '\0'; ++header, ++data_itr)
        if (*data_itr != *header)
            return false;
    return true;
}

std::string get_pic_extension(const data_view& data)
{
    const char *BMP_HEADER = "\x42\x4D",
            *PNG_HEADER = "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A",
            *JPG_HEADER = "\xFF\xD8\xFF\xE0",
            *GIF_HEADER = "\x47\x49\x46";
    if (data.check_bool(2) && compare_header(data, BMP_HEADER))
        return ".bmp";
    if (data.check_bool(8) && compare_header(data, PNG_HEADER))
        return ".png";
    if (data.check_bool(4) && compare_header(data, JPG_HEADER))
        return ".jpg";
    if (data.check_bool(3) && compare_header(data, GIF_HEADER))
        return ".gif";
    return "";
}
*/

qt_srv_interface::qt_srv_interface(asio::io_service& _main_io_service,
    asio::io_service& _misc_io_service,
    asio::ip::tcp::endpoint _local_endpoint,
    crypto::server& _crypto_srv,
    QtWindowInterface *_window,
    crypto::provider& _cryp_helper)
    :msgr_proto::server(_main_io_service, _misc_io_service, _local_endpoint, _cryp_helper, _crypto_srv), image_id(0), window(*_window)
{
    IMG_TMP_PATH = TEMP_PATH;
    IMG_TMP_PATH.mkpath(IMG_TMP_PATH_NAME);
    IMG_TMP_PATH.cd(IMG_TMP_PATH_NAME);

    QDir fs(DATA_PATH);
    if (fs.exists(publickeysFile))
    {
        std::ifstream fin(fs.filePath(publickeysFile).toLocal8Bit().data(), std::ios_base::in | std::ios_base::binary);
        std::vector<char> buf_key, buf_ex;
        char size_buf[sizeof(uint16_t)];
        fin.read(size_buf, sizeof(uint16_t));
        while (!fin.eof())
        {
            //read key
            buf_key.resize(static_cast<uint16_t>(size_buf[0]) | (size_buf[1] << 8));
            fin.read(buf_key.data(), buf_key.size());
            if (fin.eof())
                break;
            //read extra data
            fin.read(size_buf, sizeof(uint16_t));
            buf_ex.resize(static_cast<uint16_t>(size_buf[0]) | (size_buf[1] << 8));
            fin.read(buf_ex.data(), buf_ex.size());
            if (fin.eof())
                break;
            //emplace
            certifiedKeys.emplace(std::string(buf_key.data(), buf_key.size()), std::string(buf_ex.data(), buf_ex.size()));
            //read next size
            fin.read(size_buf, sizeof(uint16_t));
        }

        fin.close();
    }
    certifiedKeys.emplace(get_public_key(), "Myself");
}

qt_srv_interface::~qt_srv_interface()
{
    QDir fs(DATA_PATH);

    std::ofstream fout(fs.filePath(publickeysFile).toLocal8Bit().data(), std::ios_base::out | std::ios_base::binary);

    certifiedKeys.erase(get_public_key());

    auto itr = certifiedKeys.begin(), itrEnd = certifiedKeys.end();
    for (; itr != itrEnd; itr++)
    {
        const std::string &key = itr->first, &ex = itr->second;
        fout.put(static_cast<char>(key.size()));
        fout.put(static_cast<char>(key.size() >> 8));
        fout.write(key.data(), key.size());
        fout.put(static_cast<char>(ex.size()));
        fout.put(static_cast<char>(ex.size() >> 8));
        fout.write(ex.data(), ex.size());
    }

    fout.close();

    IMG_TMP_PATH.removeRecursively();
}

void qt_srv_interface::on_data(user_id_type id, const std::string& _data)
{
    try
    {
        data_view data(_data.data(), _data.size());

        byte type;
        data.read(type);
        switch (type)
        {
            case PAC_TYPE_MSG:
            {
                data_size_type msg_size;
                data.read(msg_size);
                std::string msg_utf8;
                data.read(msg_utf8, msg_size);

                if (data.check_bool(sizeof(data_size_type)))
                {
                    data_size_type from_size;
                    data.read(from_size);
                    std::string from;
                    data.read(from, from_size);
                    window.ExecuteHandler([this, id, msg_utf8, from](){
                        window.RecvMsg(id, msg_utf8, from);
                    });
                }
                else
                {
                    window.ExecuteHandler([this, id, msg_utf8](){
                        window.RecvMsg(id, msg_utf8, empty_string);
                    });
                }

                break;
            }
            case PAC_TYPE_FILE_H:
            {
                data_size_type blockCountAll, fNameLen;
                data.read(blockCountAll);
                data.read(fNameLen);

                data.check(fNameLen);
                QString fName = QString::fromUtf8(data.data, fNameLen);
                data.skip(fNameLen);

                int pos = fName.lastIndexOf('/');
                if (pos != -1)
                    fName.remove(0, pos + 1);
                pos = fName.lastIndexOf('\\');
                if (pos != -1)
                    fName.remove(0, pos + 1);
                if (fName.isEmpty())
                    fName = "unknown_file";

                QDir fs(DOWNLOAD_PATH);
                if (fs.exists(fName))
                {
                    int i;
                    for (i = 0; i < INT_MAX; i++)
                    {
                        if (!fs.exists(fName + "_" + QString::number(i)))
                            break;
                    }
                    if (i == INT_MAX)
                        throw(std::runtime_error("Failed to open file"));
                    fName.append("_");
                    fName.append(QString::number(i));
                }
                window.ExecuteHandler([this, id, path = fs.filePath(fName), blockCountAll](){
                    window.RecvFileH(id, path, blockCountAll);
                });

                break;
            }
            case PAC_TYPE_FILE_B:
            {
                data_size_type dataSize;
                data.read(dataSize);

                data.check(dataSize);

                std::promise<void> event_promise;
                window.ExecuteHandler([&](){
                    window.RecvFileB(id, data.data, dataSize);
                    event_promise.set_value();
                });
                event_promise.get_future().get();

                data.skip(dataSize);

                break;
            }
            case PAC_TYPE_IMAGE:
            {
                data_size_type image_size;
                data.read(image_size);

                int next_image_id;
                new_image_id(next_image_id);
                QDir image_path(IMG_TMP_PATH);
                image_path.cd(QString::number(id));
                QString imagefile_path = image_path.filePath(".messenger_tmp_" + QString::number(next_image_id));

                data.check(image_size);
                std::ofstream fout(imagefile_path.toLocal8Bit().data(), std::ios_base::out | std::ios_base::binary);
                fout.write(data.data, image_size);
                fout.close();
                data.skip(image_size);

                if (data.check_bool(sizeof(data_size_type)))
                {
                    data_size_type from_size;
                    data.read(from_size);
                    std::string from;
                    data.read(from, from_size);
                    window.ExecuteHandler([this, id, imagefile_path, from](){
                        window.RecvImg(id, imagefile_path, from);
                    });
                }
                else
                {
                    window.ExecuteHandler([this, id, imagefile_path](){
                        window.RecvImg(id, imagefile_path, empty_string);
                    });
                }


                break;
            }
            case PAC_TYPE_FEATURE_FLAG:
            {
                uint32_t flag;
                data.read(flag);
                window.ExecuteHandler([this, id, flag](){
                    window.RecvFeature(id, flag);
                });
                break;
            }
            case PAC_TYPE_PLUGIN_DATA:
            {
                char type;
                data.read(type);
                switch (type)
                {
                    case pak_file_storage:
                    {
                        uint32_t count, size;
                        std::string key_buf, name_buf;
                        std::vector<std::pair<std::string, std::string>> list;
                        data.read(count);
                        list.reserve(count);
                        for (; count > 0; count--)
                        {
                            data.read(size);
                            data.read(key_buf, size);
                            data.read(size);
                            data.read(name_buf, size);
                            list.push_back(std::make_pair(std::move(key_buf), std::move(name_buf)));
                            key_buf.clear();
                            name_buf.clear();
                        }

                        std::promise<void> event_promise;
                        window.ExecuteHandler([&](){
                            window.RecvFileList(id, list);
                            event_promise.set_value();
                        });
                        event_promise.get_future().get();

                        break;
                    }
                }

                break;
            }
            default:
            {
                break;
            }
        }
    }
    catch (qt_srv_interface_error &) {}
    catch (std::exception &ex)
    {
        on_exception(ex);
    }
    catch (...)
    {
        throw;
    }
}

void qt_srv_interface::on_join(user_id_type id, const std::string& key)
{
    IMG_TMP_PATH.mkpath(QString::number(id));

    window.ExecuteHandler([this, id, key](){
        window.Join(id, key);
    });

    uint32_t flags = feature_message_from;
    std::string flags_buf;
    flags_buf.push_back(PAC_TYPE_FEATURE_FLAG);
    for (int i = 0; i < static_cast<int>(sizeof(uint32_t)); i++)
    {
        flags_buf.push_back(static_cast<char>(flags & 0xFF));
        flags >>= 8;
    }
    send_data(id, std::move(flags_buf), msgr_proto::session::priority_sys);
}

void qt_srv_interface::on_leave(user_id_type id)
{
    window.ExecuteHandler([this, id](){
        window.Leave(id);
    });

    QDir tmp_path(IMG_TMP_PATH);
    tmp_path.cd(QString::number(id));
    tmp_path.removeRecursively();
}

bool qt_srv_interface::new_rand_port(port_type& ret)
{
    if (static_port != -1)
        ret = static_cast<port_type>(static_port);
    else
    {
        if (ports.empty())
            return false;
        std::list<port_type>::iterator portItr = ports.begin();
        for (int i = std::rand() % ports.size(); i > 0; i--)
            portItr++;
        ret = *portItr;
        ports.erase(portItr);
    }
    return true;
}

void qt_srv_interface::import_key(std::istream& fin)
{
    std::vector<char> buf_key, buf_ex;
    char size_buf[sizeof(uint16_t)];
    fin.read(size_buf, sizeof(uint16_t));
    while (!fin.eof())
    {
        //read key
        buf_key.resize(static_cast<uint16_t>(size_buf[0]) | (size_buf[1] << 8));
        fin.read(buf_key.data(), buf_key.size());
        if (fin.eof())
            break;
        //read extra data
        fin.read(size_buf, sizeof(uint16_t));
        buf_ex.resize(static_cast<uint16_t>(size_buf[0]) | (size_buf[1] << 8));
        fin.read(buf_ex.data(), buf_ex.size());
        if (fin.eof())
            break;
        //emplace
        certifiedKeys.emplace(std::string(buf_key.data(), buf_key.size()), std::string(buf_ex.data(), buf_ex.size()));
        //read next size
        fin.read(size_buf, sizeof(uint16_t));
    }
}

void qt_srv_interface::export_key(std::ostream& fout, const std::string& key)
{
    auto itr = certifiedKeys.find(key);
    if (itr == certifiedKeys.end())
        return;
    const std::string &ex = itr->second;
    fout.put(static_cast<char>(key.size()));
    fout.put(static_cast<char>(key.size() >> 8));
    fout.write(key.data(), key.size());
    fout.put(static_cast<char>(ex.size()));
    fout.put(static_cast<char>(ex.size() >> 8));
    fout.write(ex.data(), ex.size());
}

void qt_srv_interface::list_key(std::vector<key_item>& ret)
{
    for (auto itr = certifiedKeys.cbegin(), itr_end = certifiedKeys.cend(); itr != itr_end; itr++)
        ret.emplace_back(itr->first, itr->second);
}
