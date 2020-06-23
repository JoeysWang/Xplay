//
// Created by 王越 on 2020/4/29.
//

#include "IDecode.h"
#include "../XLog.h"

void IDecode::Main() {
    while (!isExit) {
        XData data;
        if (getPacketQueue()->pop(data)) {
        }
        if (sendPacket(data)) {
            while (!isExit) {
                //从解码器接收解码后的frame，此frame作为函数的输出参数供上级函数处理
                XData frame = receiveFrame();
                if (!frame.resampleData)
                    break;

                notify(frame);
            }
        }
    }
}


void IDecode::update(XData data) {
    if (data.audioOrVideo != audioOrVideo && data.audioOrVideo != -1) {
        return;
    }
    while (!isExit) {
        if (getPacketQueue()->length() < maxList) {
            //生产者，把数据压入list
            pushPacket(data);
            condition.notify_all();
            break;
        }
    }
}

FrameQueue *IDecode::getFrameQueue() const {
    return frameQueue;
}

int IDecode::pushPacket(XData data) {
    return packetQueue->push(data);

}

Queue<XData> *IDecode::getPacketQueue() const {
    return packetQueue;

}

int IDecode::getFrameSize() {
    std::unique_lock<std::mutex> lock(mutex);
    return frameQueue ? frameQueue->getFrameSize() : 0;
}


