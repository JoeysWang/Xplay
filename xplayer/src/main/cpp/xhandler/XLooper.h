//
// Created by 王越 on 2020/7/12.
//

#ifndef XPLAY_XLOOPER_H
#define XPLAY_XLOOPER_H


#include "XMessage.h"
#include "XMessageQueue.h"
#include <map>

class XLooper {
public:
    XLooper();

    void sendMessage(XMessage *);

    static XLooper *prepare();

    static void loop();

    static XLooper *myLooper();

    void quit();

    virtual ~XLooper();

private:
    void _loop();

    std::unique_ptr<XMessageQueue> queue;
    std::mutex mutex;
    bool isQuit = false;

};


#endif //XPLAY_XLOOPER_H
