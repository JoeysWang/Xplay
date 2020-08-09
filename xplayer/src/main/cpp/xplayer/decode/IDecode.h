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


private:
    void readPacket();

protected:
    std::mutex mutex;
    std::unique_ptr<Queue<PacketData>> packetQueue;
    std::unique_ptr<FrameQueue> frameQueue;
    std::shared_ptr<PlayerState> playerState;

    AVCodecContext *codecContext = nullptr;
    AVFormatContext *formatContext = nullptr;
    AVStream *stream = nullptr;
    bool isExit = false;

    DecodeMediaType decodeType;
    int mRotate;
};


#endif //FFPLAYER_IDECODE_H
