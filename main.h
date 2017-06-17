#pragma once

#ifndef _H_MAIN
#define _H_MAIN

#include "session.h"
#include "threads.h"

enum server_flags {
    feature_file_storage = 0x1,
};
enum client_flags {
    feature_message_from = 0x4,
};

struct key_item
{
    key_item() {}
    template <typename _Ty1, typename _Ty2>
    key_item(_Ty1&& _key, _Ty2&& _ex) :key(std::forward<_Ty1>(_key)), ex(std::forward<_Ty2>(_ex)) {}

    std::string key, ex;
};

struct user_ext_type
{
    //User info
    QString addr;
    std::string key, comment;
    bool have_comment = false;
    int feature = 0;

    //Logs / Chat items
    struct log_item
    {
        log_item(const QString &_from, const char* _msg) :from(_from), is_image(false), msg(_msg) {}
        log_item(const QString &_from, const std::string& _msg) :from(_from), is_image(false), msg(_msg.data()) {}
        log_item(const QString &_from, const QString& _str) :from(_from), is_image(false), msg(_str) {}
        log_item(const QString &_from, const QString& _str, bool) :from(_from), is_image(true), image(_str) {}

        QString from;
        bool is_image;
        QString msg;
        QString image;
    };
    std::list<log_item> log;

    //File transfer info
    struct send_task
    {
        send_task(std::string&& _header, const QString& path, data_size_type _blockCountAll)
            :header(_header),
            fin(path.toLocal8Bit().data(), std::ios_base::in | std::ios_base::binary),
            blockCountAll(_blockCountAll)
        {}

        std::string header;
        std::ifstream fin;
        data_size_type blockCount = 1, blockCountAll;
    };
    std::list<send_task> sendTasks;
    QString recvFile;
    int blockLast = 0;

    //Ext info
    std::vector<std::pair<std::string, std::string>> file_list;
};

class qt_srv_interface;

class QtWindowInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ get_selected WRITE select NOTIFY selectIndex)
    Q_PROPERTY(int windowWidth READ get_windowWidth WRITE set_windowWidth NOTIFY windowWidthChanged)

private:
    static constexpr int FileBlockLen = 0x80000;

public:
    QtWindowInterface();
    ~QtWindowInterface();

    void RecvMsg(user_id_type id, const std::string& msg, const std::string& from);
    void RecvImg(user_id_type id, const QString& path, const std::string& from);
    void RecvFileH(user_id_type id, const QString& file_name, size_t block_count);
    void RecvFileB(user_id_type id, const char* data, size_t size);
    void RecvFeature(user_id_type id, int flag);
    void RecvFileList(user_id_type id, std::vector<std::pair<std::string, std::string>>&);
    void Join(user_id_type id, const std::string& key);
    void Leave(user_id_type id);

    int get_selected() { return selected; }
    void select(int index) { if (selected != index) { selected = index; emit selectIndex(index); } }
    int get_windowWidth() { return window_width; }
    void set_windowWidth(int width) { if (window_width != width) { window_width = width; emit windowWidthChanged(width); } }

    Q_INVOKABLE QUrl getPicturesPath() { return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)); }
    Q_INVOKABLE QUrl getDownloadPath() { return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)); }
    Q_INVOKABLE QString urlToLocalStr(const QUrl& url) { return url.toLocalFile(); }
    Q_INVOKABLE QUrl localStrToUrl(const QString& path) { return QUrl::fromLocalFile(path); }
    Q_INVOKABLE void printDebug(const QString& msg) { qDebug(msg.toUtf8()); }
signals:
    void joined(int index, const QString& name);
    void changeName(int index, const QString& name);
    void left(int index);
    void selectIndex(int index);

    void chatClear();
    void chatText(int id, const QString& from, const QString& message);
    void chatImage(int id, const QString& from, const QString& imgPath);
    void chatFile(int id, const QString& from, const QString& filename);
    void notifyFile(int id, int progress);

    void enableFeature(int flag);
    void refreshFilelist(const QStringList& files);

    void refreshKeylist(const QStringList& key, const QStringList& ex);

    void sendFileBlock(int id);

    void windowWidthChanged(int newWidth);
public slots:
    void connectTo(const QString& addr, const QString& port);
    void disconnect();
    void sendMsg(const QString& msg);
    void sendImg(const QUrl& img_url);
    void sendFile(const QUrl& file_url);

    void reqFilelist();
    void reqDownloadFile(int index);

    void reqKeylist();
    void importKey(const QUrl& key_path);
    void exportKey(int index, const QUrl& key_path, const QString& file_name);
    void modifyKey(int index, const QString& ex);
    void trustKey();
    void distrustKey(int index);
private slots:
    void OnSelectChanged(int);
    void OnSendFileBlock(int);
private:
    QStringList GenerateFilelist();
    QStringList GenerateFilelist(user_ext_type& usr);
    void GenerateKeylist();
    void RefreshComments();

    iosrvThread threadNetwork, threadMisc;
    std::unique_ptr<crypto::server> crypto_srv;
    std::unique_ptr<qt_srv_interface> srv;

    std::unordered_map<user_id_type, user_ext_type> user_ext;

    int selected = -1;
    std::vector<user_id_type> user_id_map;

    std::vector<std::string> file_id_map;
    std::vector<key_item> key_list;

    std::unique_ptr<crypto::provider> cryp_helper;

    std::unique_ptr<char[]> file_block;

    int window_width;
};

enum pac_type {
    PAC_TYPE_MSG,
    PAC_TYPE_FILE_H,
    PAC_TYPE_FILE_B,
    PAC_TYPE_IMAGE,
    PAC_TYPE_FEATURE_FLAG,
    PAC_TYPE_PLUGIN_DATA,
};

enum plugin_pak_type {
    pak_file_storage = 0x1,
};

extern const char* IMG_TMP_PATH_NAME;
extern const char* IMG_TMP_FILE_NAME;
const size_t IMAGE_SIZE_LIMIT = 0x400000;

//Exceptions that can be safely ignored
class qt_srv_interface_error :public std::runtime_error
{
public:
    qt_srv_interface_error() :std::runtime_error("Error in qt_srv_interface") {}
};

class qt_srv_interface :public msgr_proto::server
{
public:
    qt_srv_interface(asio::io_service& _main_io_service,
        asio::io_service& _misc_io_service,
        asio::ip::tcp::endpoint _local_endpoint,
        crypto::server& _crypto_srv,
        QtWindowInterface* _window,
        crypto::provider& cryp_helper);
    ~qt_srv_interface();

    virtual void on_data(user_id_type id, const std::string& data) override;

    virtual void on_join(user_id_type id, const std::string& key) override;
    virtual void on_leave(user_id_type id) override;

    virtual bool new_rand_port(port_type& port) override;
    virtual void free_rand_port(port_type port) override { ports.push_back(port); }

    virtual bool new_key(const std::string& key) override { if (connectedKeys.count(key) > 0) return false; connectedKeys.emplace(key); return true; }
    virtual void delete_key(const std::string& key) override { connectedKeys.erase(key); }

    template <typename... _Ty>
    void certify_key(_Ty&&... key) { certifiedKeys.emplace(std::forward<_Ty>(key)...); }
    void decertify_key(const std::string& key) { if (key != get_public_key()) certifiedKeys.erase(key); }
    const std::string& get_key_ex(const std::string& key) const { return certifiedKeys.at(key); }

    void list_key(std::vector<key_item>& ret);
    void import_key(std::istream& in);
    void export_key(std::ostream& out, const std::string& key);
    template <typename _Ty>
    void edit_key(const std::string& key, _Ty&& ex) { certifiedKeys.at(key) = std::forward<_Ty>(ex); }

    void set_static_port(port_type port) { static_port = port; }
    void new_image_id(int& id) { id = image_id; image_id++; }

    virtual void on_error(const char* err) override { qWarning(QString::fromLocal8Bit(err).toUtf8()); }
private:
    const std::string empty_string;

    std::unordered_set<std::string> connectedKeys;
    std::unordered_map<std::string, std::string> certifiedKeys;
    std::list<port_type> ports;
    int static_port = -1;

    int image_id = 0;

    QtWindowInterface &window;
};

#endif
