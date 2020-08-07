//
// Created by 王越 on 2020/7/11.
//

#ifndef XPLAY_XHANDLER_H
#define XPLAY_XHANDLER_H

#include "XMessage.h"
#include "XLooper.h"

class HandleMessageI {
public:
    virtual void handleMessage(XMessage *) = 0;
};

class XHandler {
public:
    virtual ~XHandler();

public:
    HandleMessageI *callBack = nullptr;

    XLooper *looper = nullptr;
public:
    XHandler();

    XHandler(XLooper *looper);

    void setCallBack(HandleMessageI *callBack);

    void sendMessage(XMessage *message);

    void postMessage(int what, int arg1, int arg2);

    void postMessage(int what, void *obj);


    void dispatchMessage(XMessage *message);

private:
    void enqueueMessage(XMessage *);

};


#endif //XPLAY_XHANDLER_H
