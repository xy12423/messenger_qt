#pragma once

#ifndef _H_MAIN
#define _H_MAIN

#include <QObject>
#include <QString>
#include <QUrl>
#include <QStandardPaths>
#include "session.h"
#include "threads.h"

struct user_ext_type
{
    QString addr;
    struct log_type
    {
        log_type(bool _is_recv, const char* _msg) :is_recv(_is_recv), is_image(false), msg(_msg) {}
        log_type(bool _is_recv, const std::string& _msg) :is_recv(_is_recv), is_image(false), msg(_msg.data()) {}
        log_type(bool _is_recv, const QString& _str, bool _is_image = false) :is_recv(_is_recv), is_image(_is_image) { if (is_image) image = _str; else msg = _str; }

        bool is_recv, is_image;
        QString msg;
        QString image;
    };
    std::list<log_type> log;

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
    int blockLast;
};

class qt_srv_interface;

class QtWindowInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int index READ get_selected WRITE select)

private:
    static constexpr int FileBlockLen = 0x80000;

public:
    QtWindowInterface();
    ~QtWindowInterface();

    void RecvMsg(user_id_type id, const std::string& msg);
    void RecvImg(user_id_type id, const QString& path);
    void RecvFileH(user_id_type id, const QString& file_name, size_t block_count);
    void RecvFileB(user_id_type id, const char* data, size_t size);
    void Join(user_id_type id, const std::string& key);
    void Leave(user_id_type id);

    int get_selected() { return selected; }
    void select(int index) { selected = index; emit selectIndex(index); }

    Q_INVOKABLE QUrl getPicturesPath() { return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)); }
    Q_INVOKABLE QUrl getDownloadPath() { return QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)); }
    Q_INVOKABLE QString urlToLocalStr(const QUrl& url) { return url.toLocalFile(); }
signals:
    void joined(int index, const QString& name);
    void left(int index);
    void refreshChat(const QString& content);
    void selectIndex(int index);

    void sendFileBlock(int id);
public slots:
    void connectTo(const QString& addr, const QString& port);
    void disconnect();
    void sendMsg(const QString& msg);
    void sendImg(const QUrl& img_path);
    void sendFile(const QUrl& file_path);
private slots:
    void OnSelectChanged(int);
    void OnSendFileBlock(int);
private:
    QString GenerateLogStr(user_ext_type& usr);

    iosrvThread threadNetwork, threadMisc, threadCrypto;
    std::unique_ptr<crypto::server> crypto_srv;
    std::unique_ptr<qt_srv_interface> srv;

    std::unordered_map<user_id_type, user_ext_type> user_ext;

    int selected = -1;
    std::vector<user_id_type> user_id_map;

    std::unique_ptr<crypto::provider> cryp_helper;

    std::unique_ptr<char[]> file_block;
};

enum pac_type {
    PAC_TYPE_MSG,
    PAC_TYPE_FILE_H,
    PAC_TYPE_FILE_B,
    PAC_TYPE_IMAGE,
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

    virtual void on_data(user_id_type id, const std::string& data);

    virtual void on_join(user_id_type id, const std::string& key);
    virtual void on_leave(user_id_type id);

    virtual bool new_rand_port(port_type& port);
    virtual void free_rand_port(port_type port) { ports.push_back(port); }

    template <typename... _Ty>
    void certify_key(_Ty&&... key) { certifiedKeys.emplace(std::forward<_Ty>(key)...); }

    void set_static_port(port_type port) { static_port = port; }
    void new_image_id(int& id) { id = image_id; image_id++; }

    virtual void on_error(const char* err) { qDebug(err); }
private:
    std::unordered_map<std::string, std::string> certifiedKeys;
    std::list<port_type> ports;
    int static_port = -1;

    int image_id = 0;

    QtWindowInterface &window;
};

#endif
