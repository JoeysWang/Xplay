//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_HANDLERTHREAD_H
#define XPLAY_HANDLERTHREAD_H

#include "XHandler.h"
#include "ThreadUtils.h"
#include <thread>
#include <XLog.h>

class HandlerThread : public HandleMessageI {
public:
    HandlerThread() {
        std::thread thread(&HandlerThread::run, this);
        thread.detach();
    }

    virtual void onLooperPrepared() {};

    virtual ~HandlerThread() {
    }

    XLooper *getLooper() {
        if (isQuit) return nullptr;
        std::unique_lock<std::mutex> lock(internalMutex);
        while (!isQuit && mLooper == nullptr) {
            emptyLooper.wait(lock);
        }
        return mLooper;
    }

    bool quit() {
        isQuit = true;
        XLooper *looper = getLooper();
        if (looper != nullptr) {
            looper->quit();
            return true;
        }
        return false;
    }

private:
    void run() {
        mTid = ThreadUtils::currentId();
        internalMutex.lock();
        mLooper = XLooper::prepare();
        internalMutex.unlock();
        LOGI("mLooper::prepare =%p ",mLooper);
        emptyLooper.notify_all();
        onLooperPrepared();
        mLooper->loop();
        mTid = -1;
    };


private:
    bool isQuit = false;
    long mTid = -1;
    XLooper *mLooper;
    std::condition_variable emptyLooper;
    std::mutex internalMutex;

};


#endif //XPLAY_HANDLERTHREAD_H
