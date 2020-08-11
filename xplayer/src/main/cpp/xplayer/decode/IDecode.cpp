//
// Created by 王越 on 2020/8/6.
//

#include <thread>
#include "IDecode.h"
#include "XLog.h"
#include "../player/PlayerMessage.h"

IDecode::IDecode(const std::shared_ptr<PlayerState> &playerState) : playerState(playerState) {
    packetQueue = std::make_unique<Queue<PacketData *>>(100);
    frameQueue = std::make_unique<Queue<FrameData *>>(100);
}


FrameData *IDecode::currentFrame() {
    return *frameQueue->peek();
}

FrameData *IDecode::popFrame() {
    if (isExit)
        return nullptr;
    return frameQueue->pop();
}

void IDecode::popFrame(FrameData *&data) {
    if (isExit)
        return;
    frameQueue->pop(data);
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
        if (playerHandler) {
            playerHandler->postMessage(MSG_SAR_CHANGED, codecContext->width, codecContext->height);
        }
        LOGI("video rotate = %d ,width =%d , height=%d", mRotate, codecContext->width,
             codecContext->height);
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        decodeType = MEDIA_TYPE_AUDIO;
    }
    LOGI(" IDecode::openDecode open success decodeType=%d", decodeType);
    decodeThread = std::make_unique<std::thread>(&IDecode::readPacket, this);
    decodeThread->detach();
    return true;
}

void IDecode::readPacket() {
//    while (!isExit) {
//        if (isExit || !playerState) {
//            LOGI("IDecode::readPacket %s 退出 return ", tag);
//            return;
//        }
//        if (playerState->pauseRequest) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(500));
//            continue;
//        }
//        if (playerState->abortRequest) {
//            break;
//        }
    decode();
//    }


    LOGI("IDecode::readPacket %s 退出 ", tag);
}

void IDecode::pushPacket(PacketData *data) {
    if (isExit) { return; }
    packetQueue->push(data);
}

IDecode::~IDecode() {

    isExit = true;
    avcodec_free_context(&codecContext);
    codecContext = nullptr;
    formatContext = nullptr;
    stream = nullptr;
    mRotate = 0;

}

void IDecode::quit() {
    LOGI("IDecode::quit %s", tag);
    std::unique_lock<std::mutex> lock(mutex);
    isExit = true;
    packetQueue->quit();
    frameQueue->quit();
    while (isLooping) {
        loopingSignal.wait(lock);
    }

    LOGI("IDecode::quit %s success ", tag);

}

void IDecode::setPlayerHandler(const std::shared_ptr<XHandler> &playerHandler) {
    IDecode::playerHandler = playerHandler;
}

