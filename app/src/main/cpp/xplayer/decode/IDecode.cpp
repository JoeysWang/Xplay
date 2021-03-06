//
// Created by 王越 on 2020/4/29.
//

#include <thread>
#include "IDecode.h"
#include "../../XLog.h"

IDecode::IDecode(PlayerState *playerState) : playerState(playerState) {
    LOGI("IDecode  ");
}

IDecode::~IDecode() {
    LOGD("~IDecode");
    mutex.lock();
    if (packetQueue) {
        packetQueue->quit();
        packetQueue = nullptr;
    }
    if (frameQueue) {
        frameQueue->flush();
        frameQueue = nullptr;
    }
    playerHandler = nullptr;
    avcodec_close(codecContext);
    avcodec_free_context(&codecContext);
    mutex.unlock();
}

void IDecode::run() {
    while (!isExit) {
        if (playerState->abortRequest)
            return;
        if (playerState->pauseRequest) {
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        decodePacket();
    }
}

bool IDecode::openDecode(XParameter parameter, AVStream *stream, AVFormatContext *formatContext) {
    AVCodecParameters *parameters = parameter.parameters;
    if (!parameters)return false;
    this->formatCtx = formatContext;
    this->pStream = stream;
    //1.查找解码器
    AVCodec *avCodec;
    avCodec = avcodec_find_decoder_by_name("h264_mediacodec");
    if (!avCodec) {
        LOGE("avcodec_find_decoder_by_name h264_mediacodec not found " );
        avCodec = avcodec_find_decoder(parameters->codec_id);
    }

    if (!avCodec) {
        LOGE("avcodec_find_decoder error %d ", parameters->codec_id);
        return false;
    }
    LOGI("avCodec->name = %s", avCodec->name);
    //2.创建解码器上下文
    mutex.lock();
    codecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(codecContext, parameters);

    //3.打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    if (re < 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGE("avcodec_open2 error %s ", buf);
        mutex.unlock();
        return false;
    }
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        audioOrVideo = MEDIA_TYPE_VIDEO;
        AVDictionaryEntry *entry = av_dict_get(stream->metadata, "rotate", NULL,
                                               AV_DICT_MATCH_CASE);
        if (entry && entry->value) {
            mRotate = atoi(entry->value);
        } else {
            mRotate = 0;
        }
        LOGI("video rotate = %d", mRotate);
    } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        audioOrVideo = MEDIA_TYPE_AUDIO;
    }
    mutex.unlock();

    return true;
}

void IDecode::update(XData data) {
    if ((data.audioOrVideo != audioOrVideo && data.audioOrVideo != -1)
        || isExit || playerState->abortRequest) {
        return;
    }
    //生产者，把数据压入list
    pushPacket(&data);
}

FrameQueue *IDecode::getFrameQueue() const {
    return frameQueue;
}

int IDecode::pushPacket(XData *data) {
    if (!packetQueue || isExit || playerState->abortRequest)return -1;
    return packetQueue->push(*data);
}

Queue<XData> *IDecode::getPacketQueue() const {
    return packetQueue;
}

int IDecode::getFrameSize() {
    std::unique_lock<std::mutex> lock(mutex);
    return frameQueue ? frameQueue->getFrameSize() : 0;
}



