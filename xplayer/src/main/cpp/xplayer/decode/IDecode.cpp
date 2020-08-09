//
// Created by 王越 on 2020/8/6.
//

#include <thread>
#include "IDecode.h"
#include "XLog.h"
IDecode::IDecode(const std::shared_ptr<PlayerState> &playerState) : playerState(playerState) {
    packetQueue = std::make_unique<Queue<PacketData>>(100);
    frameQueue = std::make_unique<FrameQueue>(FRAME_QUEUE_SIZE, 1);

}



bool IDecode::openDecode(DecodeParam param, AVFormatContext *formatContext, AVStream *stream) {
    std::lock_guard<std::mutex> lock(mutex);
    AVCodecParameters *parameters = param.parameters;
    if (!parameters)return false;
    this->formatContext = formatContext;
    this->stream = stream;
    //1.查找解码器
    AVCodec *avCodec = avcodec_find_decoder(parameters->codec_id);

    if (!avCodec) {
        LOGE("avcodec_find_decoder error %d ", parameters->codec_id);
        return false;
    }
    LOGI("avCodec->name = %s", avCodec->name);
    //2.创建解码器上下文

    codecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(codecContext, parameters);

    //3.打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    if (re < 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGE("avcodec_open2 error %s ", buf);

        return false;
    }
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        decodeType = MEDIA_TYPE_VIDEO;
        auto *entry = av_dict_get(stream->metadata, "rotate", nullptr,
                                  AV_DICT_MATCH_CASE);
        if (entry && entry->value) {
            mRotate = atoi(entry->value);
        } else {
            mRotate = 0;
        }
        LOGI("video rotate = %d", mRotate);
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        decodeType = MEDIA_TYPE_AUDIO;
    }
    LOGI(" IDecode::openDecode open success decodeType=%d", decodeType);
    std::thread th(&IDecode::readPacket, this);
    th.detach();
    return true;
}

void IDecode::readPacket() {
    while (true) {
        if (playerState->pauseRequest) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
        if (playerState->abortRequest) {
            break;
        }
        if (decode() != 0) {
            break;
        }
    }
    LOGI("IDecode::readPacket 退出");
}

void IDecode::pushPacket(PacketData *data) {
    packetQueue->push(*data);
}

IDecode::~IDecode() {
    LOGI("IDecode::~IDecode");
    std::unique_lock<std::mutex> lock(mutex);
    codecContext = nullptr;
    formatContext = nullptr;
    stream = nullptr;
    mRotate = 0;
}

void IDecode::quit() {
    std::unique_lock<std::mutex> lock(mutex);
    isExit = true;
    packetQueue->quit();
    frameQueue->flush();
}

