//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_HANDLERTHREAD_H
#define XPLAY_HANDLERTHREAD_H

#include "XHandler.h"
#include "ThreadUtils.h"
#include <thread>
#include <memory>
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

    std::shared_ptr<XLooper> getLooper() {
        if (isQuit) return nullptr;
        std::unique_lock<std::mutex> lock(internalMutex);
        while (!isQuit && mLooper == nullptr) {
            emptyLooper.wait(lock);
        }
        return mLooper;
    }

    bool quit() {
        isQuit = true;
        auto looper = getLooper();
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
        LOGI("mLooper::prepare ");
        emptyLooper.notify_all();
        onLooperPrepared();
        mLooper->loop();
        mTid = -1;
    };


private:
    std::atomic_bool isQuit{false};
    long mTid = -1;
    std::shared_ptr<XLooper> mLooper;
    std::condition_variable emptyLooper;
    std::mutex internalMutex;

};


#endif //XPLAY_HANDLERTHREAD_H
