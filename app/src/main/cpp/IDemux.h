//
// Created by 王越 on 2020/4/26.
//

#ifndef XPLAY_IDEMUX_H
#define XPLAY_IDEMUX_H

#include "XData.h"
#include "XThread.h"

//解封装接口
class IDemux : public XThread {

public:
    //打开文件、流媒体 http rtsp
    virtual bool open(const char *url) = 0;

    //读取一帧数据，数据由调用者清理
    virtual XData read() = 0;

    //总时长
    int totalMs = 0;
protected:
    virtual void Main();
};


#endif //XPLAY_IDEMUX_H
