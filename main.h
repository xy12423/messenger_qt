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
        enum log_item_type {
            ITEM_TEXT,
            ITEM_IMAGE,
            ITEM_FILE,
        };

        log_item(const QString &_from, const char* _msg) :from(_from), type(ITEM_TEXT), msg(_msg) {}
        log_item(const QString &_from, const std::string& _msg) :from(_from), type(ITEM_TEXT), msg(_msg.data()) {}
        log_item(const QString &_from, const QString& _str) :from(_from), type(ITEM_TEXT), msg(_str) {}
        log_item(const QString &_from, const QString& _str, bool) :from(_from), type(ITEM_IMAGE), image(_str) {}
        log_item(const QString &_from, const QString& _str, int _id) :from(_from), type(ITEM_FILE), fileName(_str), fileID(_id) {}

        QString from;
        log_item_type type;

        QString msg;
        QString image;
        QString fileName;
        int fileID;
    };
    std::vector<log_item> log;

    //Ext info
    std::vector<std::pair<std::string, std::string>> file_list;
};

struct userFileTransferInfo
{
    struct sendTask
    {
        sendTask(const std::string& _header, const QString& path, data_size_type _blockCountAll, int _id)
            :header(_header),
            fin(path.toLocal8Bit().data(), std::ios_base::in | std::ios_base::binary),
            blockCountAll(_blockCountAll),
            sendID(_id)
        {}

        std::string header;
        std::ifstream fin;
        data_size_type blockCount = 0, blockCountAll;
        int sendID;
    };
    std::list<sendTask> sendTasks;

    QString recvFile;
    int recvID = -1, blockAll = 0, blockLast = 0;
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
    QtWindowInterface(QGuiApplication& app);
    ~QtWindowInterface();

    void RecvMsg(user_id_type id, const std::string& msg, const std::string& from);
    void RecvImg(user_id_type id, const QString& path, const std::string& from);
    void RecvFileH(user_id_type id, const QString& file_path, size_t block_count);
    void RecvFileB(user_id_type id, const char* data, size_t size);
    void RecvFeature(user_id_type id, int flag);
    void RecvFileList(user_id_type id, std::vector<std::pair<std::string, std::string>>&);
    void Join(user_id_type id, const std::string& key);
    void Leave(user_id_type id);

    void ExecuteHandler(std::function<void()>&& func) { emit executeFunc(std::make_shared<std::function<void()>>(std::move(func))); }
    void ExecuteHandlerMisc(std::function<void()>&& func) { threadMisc.get_io_service().post(std::move(func)); }

    int get_selected() { return selected; }
    void select(int index) { if (selected != index) { selected = index; emit selectIndex(index); } }
    int get_windowWidth() { return window_width; }
    void set_windowWidth(int width) { if (window_width != width) { window_width = width; emit windowWidthChanged(width); } }

    Q_INVOKABLE QUrl getPicturesPath() { return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)); }
    Q_INVOKABLE QUrl getDownloadPath() { return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)); }
    Q_INVOKABLE QString urlToLocalStr(const QUrl& url) { return url.toLocalFile(); }
    Q_INVOKABLE QUrl localStrToUrl(const QString& path) { return QUrl::fromLocalFile(path); }
    Q_INVOKABLE void print(const QString& msg) { emit printMessage(msg); }
    Q_INVOKABLE void setClipboard(const QString& str) { QGuiApplication::clipboard()->setText(str); }

#ifdef ANDROID
    Q_INVOKABLE void notify(const QString& title, const QString& msg) { notifyMessage(title, msg); }
    Q_INVOKABLE void notifyClear() { notifyMessageClear(); }
#else
    Q_INVOKABLE void notify(const QString&, const QString&) {}
    Q_INVOKABLE void notifyClear() {}
#endif

    Q_INVOKABLE void printDebug(const QString& msg) { qDebug(msg.toUtf8()); }

signals:
    //Basic sig
    void joined(int index, const QString& name);
    void changeName(int index, const QString& name);
    void left(int index);
    void selectIndex(int index);

    void executeFunc(const std::shared_ptr<std::function<void()>>& funcPtr);

    //Chat items
    void chatClear();
    void chatText(int id, const QString& from, const QString& message);
    void chatImage(int id, const QString& from, const QString& imgPath);
    void chatFile(int id, const QString& from, const QString& filename);
    void notifyFileProgress(int fid, int progress);

    //Exts
    void enableFeature(int flag);
    void refreshFilelist(const QStringList& files);
    void refreshKeylist(const QStringList& key, const QStringList& ex);

    //Others
    void refreshConnHistory(const QStringList& addr, const QStringList& port);

    void windowWidthChanged(int newWidth);
    void printMessage(const QString& msg);
public slots:
    //Basic op
    void connectTo(const QString& addr, const QString& port);
    void disconnect();
    void sendMsg(const QString& msg);
    void sendImg(const QUrl& img_url);
    void sendFile(const QUrl& file_url);
    void reqFileProgress(int id);

    //Ext::file_storage
    void reqFilelist();
    void reqDownloadFile(int index);

    //Ext:key_manager
    void reqKeylist();
    void importKey(const QUrl& key_path);
    void exportKey(int index, const QUrl& key_path, const QString& file_name);
    void modifyKey(int index, const QString& ex);
    void trustKey();
    void distrustKey(int index);

    //Others
    void reqConnHistory();
    void reqConnHistoryDel(int index);

    void onApplicationStateChanged(Qt::ApplicationState state);
private slots:
    void onSelectChanged(int);
    void onExecuteFunc(const std::shared_ptr<std::function<void()>>& funcPtr);
private:
    QStringList GenerateFilelist();
    QStringList GenerateFilelist(user_ext_type& usr);
    void GenerateKeylist();
    void RefreshComments();
    void SendFileBlock(int);

    void LoadConnHistory();
    void SaveConnHistory();
    void AddConnHistory(const QString& addr, port_type port);

#ifdef ANDROID
    void notifyMessage(const QString& title, const QString& msg);
    void notifyMessageClear();
#endif

    iosrvThread threadNetwork, threadMisc;
    std::unique_ptr<crypto::provider> cryp_helper;
    std::unique_ptr<crypto::server> crypto_srv;
    std::unique_ptr<qt_srv_interface> srv;

    //Basic data
    std::unordered_map<user_id_type, user_ext_type> user_ext;
    std::unordered_map<user_id_type, userFileTransferInfo> userFileTrans;
    std::vector<int> fileProgress;

    int selected = -1;
    std::vector<user_id_type> user_id_map;

    //Ext data
    std::vector<std::string> file_id_map;
    std::vector<key_item> key_list;

    std::unique_ptr<char[]> file_block;

    std::vector<std::pair<QString, port_type>> connHistory;
    int window_width;
    bool is_in_background = false;
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
    void new_image_id(int& id) { id = image_id++; }

    virtual void on_error(const char* err) override { qWarning(QString::fromLocal8Bit(err).toUtf8()); }
private:
    const std::string empty_string;

    std::unordered_set<std::string> connectedKeys;
    std::unordered_map<std::string, std::string> certifiedKeys;
    std::list<port_type> ports;
    int static_port = -1;

    std::atomic_int image_id;

    QtWindowInterface &window;
};

inline uint16_t u16_from_data(char* data)
{
    return static_cast<uint16_t>(static_cast<uint8_t>(data[0]) | (static_cast<uint32_t>(static_cast<uint8_t>(data[1])) << 8));
}

extern QString TEMP_PATH, DATA_PATH, DOWNLOAD_PATH;

#endif
