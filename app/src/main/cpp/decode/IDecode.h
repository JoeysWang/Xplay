//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H

#include <list>
#include "../IObserver.h"
#include "../data/XParameter.h"
#include "../queue/Queue.h"

extern "C" {
#include "libavformat/avformat.h"
};
struct XData;

#include "../queue/FrameQueue.h"
#include "../queue/PacketQueue.h"
#include "../player/PlayerState.h"
#include "../sync/MediaClock.h"

enum DecodeMediaType {
    MEDIA_TYPE_NONE = -1,
    MEDIA_TYPE_AUDIO = 0,
    MEDIA_TYPE_VIDEO = 1
};

//解码接口 支持硬解码
class IDecode : public IObserver {
public:
    //打开解码器
      bool openDecode(XParameter parameter,
                            AVStream *stream,
                            AVFormatContext *formatContext) ;

//    //future模型, 发送数据到线程解码
//    virtual bool sendPacket(XData pkt) = 0;
//
//    //从线程获取解码结果，线程不安全
//    virtual XData receiveFrame() = 0;

    virtual int decodePacket() = 0;

    //生产数据，如果是满的，阻塞
    void update(XData data) override;

    FrameQueue *getFrameQueue() const;

    Queue<XData> *getPacketQueue() const;

    int pushPacket(XData data);

    int getFrameSize();

public:
    //Audio =0
    DecodeMediaType audioOrVideo = MEDIA_TYPE_NONE;

    //最大队列缓冲
    int maxList = 100;

    PlayerState *playerState;
    AVStream *pStream = nullptr;
    MediaClock *masterClock = nullptr;

    AVCodecContext *codecContext =nullptr;
    AVFormatContext *formatCtx = nullptr;


protected:
    //消费数据，如果是空的，阻塞
    virtual void Main() override;

    //所有的读取缓冲
//    std::list<XData> packets;
    std::mutex decodeMutex;

    long next_pts;
    AVRational next_pts_tb;

    AVRational stream_time_base;

    FrameQueue *frameQueue;

    Queue<XData> *packetQueue;       // 数据包队列
};


#endif //XPLAY_IDECODE_H
