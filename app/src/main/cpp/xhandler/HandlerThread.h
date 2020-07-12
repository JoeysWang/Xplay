//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_HANDLERTHREAD_H
#define XPLAY_HANDLERTHREAD_H

#include "XHandler.h"
#include <thread>

class HandlerThread : public HandleMessageI {
public:
    HandlerThread() {
        auto looper = XLooper::prepare();
        handler = new XHandler();
        handler->setCallBack(this);
        std::thread thread(&HandlerThread::run, this);
    }

    virtual ~HandlerThread() {
        delete handler;
    }

    XHandler *getHandler() const {
        return handler;
    }

private:
    virtual void run() {
        XLooper::loop();
    };

    virtual void handleMessage(XMessage *message) override = 0;

    virtual void stop() {
        isQuit = true;
    }

private:
    bool isQuit = false;
    XHandler *handler;


};


#endif //XPLAY_HANDLERTHREAD_H
