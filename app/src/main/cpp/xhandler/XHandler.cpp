//
// Created by 王越 on 2020/7/11.
//

#include <string>
#include <thread>
#include "XHandler.h"
#include "../../XLog.h"
#include "LooperManager.h"
#include "ThreadUtils.h"

XHandler::XHandler() {
    looper = XLooper::myLooper();
}

void XHandler::sendMessage(XMessage *message) {
    message->target = this;
    enqueueMessage(message);
}

void XHandler::dispatchMessage(XMessage *message) {
    if (callBack) {
        callBack->handleMessage(message);
    }
}

void XHandler::enqueueMessage(XMessage *message) {
    if (looper) {
        looper->sendMessage(message);
    }
}

XHandler::~XHandler() {
    looper->quit();
    LooperManager::getInstance()->deleteLooper(ThreadUtils::currentId());
    looper = nullptr;
}

void XHandler::setCallBack(HandleMessageI *callBack) {
    XHandler::callBack = callBack;
}
