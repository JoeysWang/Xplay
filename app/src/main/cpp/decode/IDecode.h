//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H

#include <list>
#include "../IObserver.h"
#include "../data/XParameter.h"


struct XData;
#include "../queue/FrameQueue.h"
#include "../queue/PacketQueue.h"

//解码接口 支持硬解码
class IDecode : public IObserver {
public:
    //打开解码器
    virtual bool open(XParameter parameter, bool isHard = false) = 0;

    //future模型, 发送数据到线程解码
    virtual bool sendPacket(XData pkt) = 0;

    //从线程获取解码结果，线程不安全
    virtual XData receiveFrame() = 0;

    //生产数据，如果是满的，阻塞
    void update(XData data) override;

    FrameQueue *getFrameQueue() const;

    PacketQueue *getPacketQueue() const;

    int getFrameSize();

public:
    //Audio =0
    int audioOrVideo = -1;

    //同步时间，再次打开文件要清理
    int syncAudioPts = 0;
    //当前播放到的位置
    int videoPts=0;

    //最大队列缓冲
    int maxList = 100;

protected:
    //消费数据，如果是空的，阻塞
    virtual void Main() override;

    //所有的读取缓冲
    std::list<XData> packets;
    std::mutex packetMutex;

    FrameQueue *frameQueue;
    PacketQueue *packetQueue;       // 数据包队列
};


#endif //XPLAY_IDECODE_H
