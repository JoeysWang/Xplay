//
// Created by 王越 on 2020/7/12.
//

#include <mutex>
#include "XLooper.h"
#include "XLog.h"
#include "LooperManager.h"
#include "ThreadUtils.h"
#include "XHandler.h"

XLooper::XLooper() {
    queue = new XMessageQueue(10);
}

XLooper::~XLooper() {
    delete (queue);
}

void XLooper::sendMessage(XMessage *message) {
    std::unique_lock<std::mutex> lock(mutex);
    if (isQuit) { return; }
    queue->push(*message);
}

XLooper *XLooper::prepare() {
    return LooperManager::getInstance()->createLooper(ThreadUtils::currentId());
}

void XLooper::loop() {
    XLooper *looper = myLooper();
    LOGI(" XLooper::loop %p", looper);
    if (looper) {
        looper->_loop();
    }
}

XLooper *XLooper::myLooper() {
    return LooperManager::getInstance()->getLooper(ThreadUtils::currentId());
}

void XLooper::_loop() {
    for (;;) {
        if (isQuit) { return; }
        auto message = new XMessage;

        if (queue->pop(*message)) {
            if (message->target) {
                message->target->dispatchMessage(message);
            }
        }

    }
}

