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
public slots:
    void run_iosrv() {
        bool ok;
        do
        {
            ok = true;
            try
            {
                iosrv.run();
            }
            catch (...) { ok = false; }
        }
        while (!ok);
        emit on_stop();
    }
signals:
    void on_stop();
private:
    asio::io_service& iosrv;
};

class iosrvThread : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    iosrvThread() {
        iosrvWorker *worker = new iosrvWorker(iosrv);
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

    void start() { iosrv_work = std::make_shared<asio::io_service::work>(iosrv); emit do_start(); stopped = false; }
    void stop() { iosrv_work.reset(); }
    bool is_stopped() { return stopped; }

    asio::io_service& get_io_service() { return iosrv; }
signals:
    void do_start();
public slots:
    void on_stop() { stopped = true; }
private:
    asio::io_service iosrv;
    std::shared_ptr<asio::io_service::work> iosrv_work;
    volatile bool stopped = true;
};

#endif
