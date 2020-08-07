//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_IDECODE_H
#define XPLAY_IDECODE_H

#include <list>
#include "../IObserver.h"
#include "../data/XParameter.h"
#include "../queue/Queue.h"
#include <memory>

extern "C" {
#include "libavformat/avformat.h"
};
struct XData;

#include "../queue/FrameQueue.h"
#include "../queue/PacketQueue.h"
#include "../player/PlayerState.h"
#include "../sync/MediaClock.h"
#include "../../xhandler/HandlerThread.h"

enum DecodeMediaType {
    MEDIA_TYPE_NONE = -1,
    MEDIA_TYPE_AUDIO = 0,
    MEDIA_TYPE_VIDEO = 1
};

//解码接口 支持硬解码
class IDecode : public IObserver {
public:
    IDecode(PlayerState *playerState);

    virtual ~IDecode();

    //打开解码器
    bool openDecode(XParameter parameter,
                    AVStream *stream,
                    AVFormatContext *formatContext);

    virtual int decodePacket() = 0;

    //生产数据，如果是满的，阻塞
    void update(XData data) override;

//    FrameQueue *getFrameQueue() const;

    XData *currentFrame();

    void *popFrame();


    int pushPacket(XData *data);

    int getFrameSize();

public:
    //Audio =0
    DecodeMediaType audioOrVideo = MEDIA_TYPE_NONE;

    PlayerState *playerState;
    AVStream *pStream = nullptr;

    AVCodecContext *codecContext = nullptr;
    AVFormatContext *formatCtx = nullptr;

    XHandler *playerHandler = nullptr;


protected:
    //消费数据，如果是空的，阻塞
    virtual void run() override;

    FrameQueue *frameQueue;
    Queue<XData> *packetQueue;

    int mRotate;
};


#endif //XPLAY_IDECODE_H
