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

void XHandler::postMessage(int what, int arg1, int arg2) {
    auto xMessage = new XMessage;
    xMessage->what = what;
    xMessage->arg1 = arg1;
    xMessage->arg2 = arg2;
    sendMessage(xMessage);
}

void XHandler::dispatchMessage(XMessage *message) {
    if (callBack) {
        callBack->handleMessage(message);
    } else {
        LOGE("dispatchMessage callBack is null");
    }
}

void XHandler::enqueueMessage(XMessage *message) {
    if (looper) {
        looper->sendMessage(message);
    } else {
        LOGE("enqueueMessage looper is null");
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

