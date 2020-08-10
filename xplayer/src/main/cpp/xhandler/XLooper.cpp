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
    queue = std::make_unique<XMessageQueue>(10);
}

XLooper::~XLooper() {
    LOGI("XLooper::~XLooper");
}

void XLooper::quit() {
    if (isQuit) { return; }
    std::unique_lock<std::mutex> lock(mutex);
    isQuit = true;
    queue->quit();
    LooperManager::getInstance()->deleteLooper(ThreadUtils::currentId());
}

void XLooper::sendMessage(XMessage *message) {
    std::unique_lock<std::mutex> lock(mutex);
    if (isQuit) { return; }
    queue->push(message);
}

std::shared_ptr<XLooper> XLooper::prepare() {
    return LooperManager::getInstance()->createLooper(ThreadUtils::currentId());
}

void XLooper::loop() {
    std::shared_ptr<XLooper> looper = myLooper();
    if (looper) {
        looper->_loop();
    }
}

std::shared_ptr<XLooper> XLooper::myLooper() {
    return LooperManager::getInstance()->getLooper(ThreadUtils::currentId());
}

void XLooper::_loop() {
    for (;;) {
        if (isQuit) {
            break;
        }
        auto message = queue->pop();
        if (message != nullptr) {
            if (message->target) {
                message->target->dispatchMessage(message);
            }
        }
    }

    LOGI("XLooper::_loop 退出 success");
}

