//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_IDECODE_H
#define FFPLAYER_IDECODE_H

#include <memory>
#include "../dataStructure/Queue.h"
#include "../data/PacketData.h"
#include "../data/PlayerState.h"
#include "../data/DecodeParam.h"
#include "../dataStructure/FrameQueue.h"
#include "../../xhandler/XHandler.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

enum DecodeMediaType {
    MEDIA_TYPE_NONE = -1,
    MEDIA_TYPE_AUDIO = 0,
    MEDIA_TYPE_VIDEO = 1
};

class IDecode {

public:
    IDecode(const std::shared_ptr<PlayerState> &playerState);

    bool openDecode(DecodeParam param, AVFormatContext *formatContext, AVStream *stream);

    /**
     * 真正解码
     * @return 0 = success, <0 = error
     */
    virtual int decode() = 0;

    void pushPacket(PacketData *data);

    void quit();

    virtual ~IDecode();

    FrameData *currentFrame();

    FrameData *popFrame();

    void popFrame(FrameData *&);

    void setPlayerHandler(const std::shared_ptr<XHandler> &playerHandler);

private:
    void readPacket();

public:
    AVCodecContext *codecContext = nullptr;
    AVFormatContext *formatContext = nullptr;
protected:
    std::mutex mutex;
    std::unique_ptr<Queue<PacketData *>> packetQueue;
    std::unique_ptr<std::thread> decodeThread;
//    std::unique_ptr<FrameQueue> frameQueue;
    std::unique_ptr<Queue<FrameData *>> frameQueue;
    std::shared_ptr<PlayerState> playerState;
    std::shared_ptr<XHandler> playerHandler;

    const char *tag = "";

    AVStream *stream = nullptr;
    std::atomic_bool isExit{false};
    bool isLooping = false;
    std::condition_variable loopingSignal;


    DecodeMediaType decodeType;
    int mRotate;
};


#endif //FFPLAYER_IDECODE_H
