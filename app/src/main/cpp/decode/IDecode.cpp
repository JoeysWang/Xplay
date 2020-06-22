//
// Created by 王越 on 2020/4/29.
//

#include "IDecode.h"
#include "../XLog.h"

void IDecode::Main() {
    while (!isExit) {
        packetMutex.lock();
        if (packets.empty()) {
            //如果是空的，一直循环
            packetMutex.unlock();
            XSleep(1);
            continue;
        }


        if (audioOrVideo == 1 && syncAudioPts > 0) {
            //音视频同步,声音pts<当前pts，等待
            if (syncAudioPts < videoPts) {
                packetMutex.unlock();
                XSleep(1);
                continue;
            }
        }
        //消费者，把数据取出list
        //如果不是空的，处理数据：
        XData data = packets.front();
        packets.pop_front();
        //一个数据包，可能会获得多个frame
        if (sendPacket(data)) {

            while (!isExit) {
                XData frame = receiveFrame();
                if (!frame.data)break;

                videoPts = frame.pts;
                //发数据给观察者
//                LOGD(" receiveFrame  size %d ,frame is audio= %d", frame.size, frame.audioOrVideo);
                notify(frame);
            }
        }
        data.drop();
        packetMutex.unlock();
    }
}

void IDecode::update(XData data) {
    if (data.audioOrVideo != audioOrVideo && data.audioOrVideo != -1) {
        return;
    }
    while (!isExit) {
        packetMutex.lock();
        if (packets.size() < maxList) {
            //生产者，把数据压入list
            packets.push_back(data);
            packetMutex.unlock();
            break;
        }
        packetMutex.unlock();
        XSleep(1);
    }
}

FrameQueue *IDecode::getFrameQueue() const {
    return frameQueue;
}

PacketQueue *IDecode::getPacketQueue() const {
    return packetQueue;
}

int IDecode::getFrameSize() {
    std::unique_lock<std::mutex> lock(mutex);
    return frameQueue ? frameQueue->getFrameSize() : 0;
}


