//
// Created by 王越 on 2020/4/29.
//

#include "IDecode.h"
#include "../XLog.h"

void IDecode::Main() {
    while (!isExit) {
//        XData data;
//        if (getPacketQueue()->pop(data)) {
//        }
//        if (sendPacket(data)) {
//            while (!isExit) {
//            //从解码器接收解码后的frame，此frame作为函数的输出参数供上级函数处理
//            XData frame = receiveFrame();
//            if (!frame.resampleData)
//                break;
//
//            notify(frame);
//            }
//        }
        decodePacket();
    }
}

bool IDecode::openDecode(XParameter parameter, AVStream *stream, AVFormatContext *formatContext) {
    AVCodecParameters *parameters = parameter.parameters;
    if (!parameters)return false;
    this->formatCtx = formatContext;
    this->pStream = stream;
    //1.查找解码器
    AVCodec *avCodec = avcodec_find_decoder(parameters->codec_id);
    LOGI("avcodec_find_decoder video codec_id=%d ", parameters->codec_id);

    if (!avCodec) {
        LOGE("avcodec_find_decoder error %d ", parameters->codec_id);
        return false;
    }

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
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO)
        audioOrVideo = MEDIA_TYPE_VIDEO;
    else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO)
        audioOrVideo = MEDIA_TYPE_AUDIO;
    mutex.unlock();

    return true;
}

void IDecode::update(XData data) {
    if (data.audioOrVideo != audioOrVideo && data.audioOrVideo != -1) {
        return;
    }
    //生产者，把数据压入list
    pushPacket(data);

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


