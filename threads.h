#pragma once

#ifndef _H_THREADS
#define _H_THREADS

#include "stdafx.h"
#include <QThread>

class iosrvWorker : public QObject
{
    Q_OBJECT

public:
    iosrvWorker(asio::io_service& _iosrv): iosrv(_iosrv) {}

    void setWork(const std::shared_ptr<asio::io_service::work>& work) { watcher = work; }
public slots:
    void run_iosrv() {
        while (!watcher.expired())
        {
            try
            {
                iosrv.run();
            }
            catch (...) {}
        }
        emit on_stop();
    }
signals:
    void on_stop();
private:
    asio::io_service& iosrv;
    std::weak_ptr<asio::io_service::work> watcher;
};

class iosrvThread : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    iosrvThread() {
        worker = new iosrvWorker(iosrv);
        worker->moveToThread(&workerThread);
        connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
        connect(this, SIGNAL(do_start()), worker, SLOT(run_iosrv()));
        connect(worker, SIGNAL(on_stop()), this, SLOT(on_stop()), Qt::DirectConnection);
        workerThread.start();

    }
    ~iosrvThread() {
        if (iosrv_work)
            iosrv_work.reset();
        workerThread.quit();
        workerThread.wait();
    }

    void start() { iosrv_work = std::make_shared<asio::io_service::work>(iosrv); worker->setWork(iosrv_work); stopped = false; emit do_start(); }
    void stop() { iosrv_work.reset(); }
    bool is_stopped() { return stopped; }

    asio::io_service& get_io_service() { return iosrv; }
signals:
    void do_start();
public slots:
    void on_stop() { stopped = true; }
private:
    asio::io_service iosrv;
    iosrvWorker *worker;
    std::shared_ptr<asio::io_service::work> iosrv_work;
    volatile bool stopped = true;
};

#endif
