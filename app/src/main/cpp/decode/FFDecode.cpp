//
// Created by 王越 on 2020/4/29.
//

#include "FFDecode.h"
#include "../XLog.h"
#include "../data/XParameter.h"
#include "../data/XData.h"

extern "C" {
#include "libavcodec/avcodec.h"
}

bool FFDecode::open(XParameter parameter) {
    AVCodecParameters *parameters = parameter.parameters;
    if (!parameters)return false;

    //1.查找解码器
    AVCodec *avCodec = avcodec_find_decoder(parameters->codec_id);
    if (!avCodec) {
        LOGE("avcodec_find_decoder error %d ", parameters->codec_id);
        return false;
    }
    LOGI("avcodec_find_decoder success! ");

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
    LOGI("avcodec_open2 success! ");


    return true;
}

bool FFDecode::sendPacket(XData *pkt) {
    if (!codecContext) {
        return false;
    }
    if (!pkt || !pkt->data) {
        return false;
    }

    codecContext->thread_count = 8;
    int re = avcodec_send_packet(codecContext, (AVPacket *) pkt->data);
    if (re != 0) {
        LOGE("avcodec_send_packet failed ");
        return false;
    }


    return true;
}

XData FFDecode::receiveFrame() {
    if (!codecContext) {
        return XData();
    }

    if (!avFrame) {
        avFrame = av_frame_alloc();
    }

    int re = avcodec_receive_frame(codecContext, avFrame);
    if (re != 0) {
        LOGE("解码失败 avcodec_receive_frame");
        return XData();
    }
    XData d;
    d.data = (unsigned char *) (avFrame);

    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO)
        d.size = (avFrame->linesize[0] +
                  avFrame->linesize[1] +
                  avFrame->linesize[2]) * avFrame->height;


    return d;
}
