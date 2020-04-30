//
// Created by 王越 on 2020/4/26.
//
#include <jni.h>
#include "FFDemux.h"
#include "../XLog.h"
#include "../data/XParameter.h"

extern "C" {
#include <libavformat/avformat.h>
}


//打开文件、流媒体 http rtsp
bool FFDemux::open(const char *url) {
    LOGD("open %s", url);
    int re = avformat_open_input(&ic, url, 0, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("open ffdemux failed %s", url);
        return false;
    }
    re = avformat_find_stream_info(ic, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("avformat_find_stream_info failed %s", url);
        return false;
    }
//    getVideoParameter();
//    getAudioParameter();
    return true;
}

//读取一帧数据，数据由调用者清理
XData FFDemux::read() {

    if (!ic)
        return XData();
    XData d;
    AVPacket *avPacket = av_packet_alloc();
    int re = av_read_frame(ic, avPacket);
    if (re != 0) {
        av_packet_free(&avPacket);
        return XData();
    }
    d.data = (unsigned char *) (avPacket);
    d.size = avPacket->size;

    if (avPacket->stream_index == audioStreamIndex)
        d.audioOrVideo = 0;
    else if (avPacket->stream_index == videoStreamIndex)
        d.audioOrVideo = 1;
    else {
        av_packet_free(&avPacket);
        return XData();
    }
    return d;
}

XParameter FFDemux::getVideoParameter() {
    if (!ic) {
        LOGE("get video param failed ic is null");
        return XParameter();
    }
    //获取视频流索引
    int videoIndex = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (videoIndex < 0) {
        LOGE("av_find_best_stream video is empty");
        return XParameter();
    }
    this->videoStreamIndex = videoIndex;

    XParameter para;
    para.parameters = ic->streams[videoIndex]->codecpar;
    LOGI("getVideoParameter success codec_id= %d",para.parameters->codec_id);
    return para;
}

XParameter FFDemux::getAudioParameter() {
    if (!ic) {
        LOGE("get audio param failed ic is null");
        return XParameter();
    }
    //获取音频流索引
    int audioIndex = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (audioIndex < 0) {
        LOGE("av_find_best_stream audio is empty");
        return XParameter();
    }
    this->audioStreamIndex = audioIndex;
    LOGI("getAudioParameter success");

    XParameter para;
    para.parameters = ic->streams[audioIndex]->codecpar;
    return para;
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {

        isFirst = false;
        //注册所有解封装器
        av_register_all();

        avcodec_register_all();

        avformat_network_init();
        LOGI("regsist ffmpeg");
    }
}

FFDemux::~FFDemux() {
    delete ic;
    LOGI("~FFDemux");
}

