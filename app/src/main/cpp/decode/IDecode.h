//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H

#include "../IObserver.h"
#include "../data/XParameter.h"

struct XData;

//解码接口 支持硬解码
class IDecode : public IObserver {
public:
    //打开解码器
    virtual bool open(XParameter parameter) = 0;

    //future模型, 发送数据到线程解码
    virtual bool sendPacket(XData *pkt) = 0;

    //从线程获取解码结果
    virtual XData receiveFrame() = 0;

protected:
    virtual void Main();
};


#endif //XPLAY_IDECODE_H
