//
// Created by 王越 on 2020/4/29.
//

#include "FFDecode.h"
#include "../XLog.h"
#include "../data/XParameter.h"

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
