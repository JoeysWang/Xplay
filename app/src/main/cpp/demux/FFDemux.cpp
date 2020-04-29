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
//    LOGI("pack size %d ,ptss %ld", avPacket->size, avPacket->pts);
    d.data = (avPacket);
    d.size = avPacket->size;


    return d;
}

XParameter FFDemux::getVideoParamter() {
    if (!ic){
        LOGE("get video param failed ic is null");
        return XParameter();
    }
    //获取视频流索引
    int videoIndex = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (videoIndex < 0) {
        LOGE("av_find_best_stream video is empty");
        return XParameter();
    }

    XParameter para;
    para.parameters = ic->streams[videoIndex]->codecpar;
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

