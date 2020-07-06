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

FFDemux::FFDemux(PlayerState *playerState) : IDemux(playerState) {
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

//打开文件、流媒体 http rtsp
bool FFDemux::open(const char *url) {
    close();
    mutex.lock();
    LOGD("open %s", url);
    int re = avformat_open_input(&formatContext, url, 0, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("open ffdemux failed %s", url);
        mutex.unlock();
        return false;
    }
    re = avformat_find_stream_info(formatContext, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("avformat_find_stream_info failed %s", url);
        mutex.unlock();
        return false;
    }
    mutex.unlock();

    getVideoParameter();
    getAudioParameter();
    return true;
}

//读取一帧数据，数据由调用者清理
XData FFDemux::read() {
    mutex.lock();
    if (!formatContext) {
        mutex.unlock();
        return XData();
    }
    XData d;
    AVPacket *avPacket = av_packet_alloc();
    int re = av_read_frame(formatContext, avPacket);
    if (re != 0) {
        LOGE("av_read_frame error %s", av_err2str(re));
        av_packet_free(&avPacket);
        if (re == AVERROR_EOF) {
            LOGE("ffdemux  read 到结尾了 结束 %s", av_err2str(re));
            stop();
        }
        mutex.unlock();
        return XData();
    }
    d.packet = avPacket;
    d.size = avPacket->size;

    if (avPacket->stream_index == audioStreamIndex)
        d.audioOrVideo = 0;
    else if (avPacket->stream_index == videoStreamIndex)
        d.audioOrVideo = 1;
    else {
        LOGE("FFDemux::read failed avPacket->stream_index 未知");
        av_packet_free(&avPacket);
        mutex.unlock();
        return XData();
    }
    //转换pts
    AVStream *pStream = formatContext->streams[avPacket->stream_index];
    double ptsSeconds =
            avPacket->pts * av_q2d(pStream->time_base) *
            1000;//demux出来的帧的pts：是相对于源AVStream的timebase

    d.pts = avPacket->pts;//demux出来的帧的pts：是相对于源AVStream的timebase
    AVRational frame_rate = av_guess_frame_rate(formatContext,
                                                pStream,
                                                NULL);
    d.frame_rate = frame_rate;
    d.time_base = pStream->time_base;

//    LOGI("duration =%f", pStream->duration * av_q2d(pStream->time_base));
//    LOGI("pStream->time_base  =%d/%d", pStream->time_base.num,
//         pStream->time_base.den);
//    LOGI("FFDemux::read success pauseRequest=%d",playerState->pauseRequest);
    mutex.unlock();
    return d;
}

XParameter FFDemux::getVideoParameter() {
    mutex.lock();
    if (!formatContext) {
        LOGE("get video param failed ic is null");
        mutex.unlock();
        return XParameter();
    }
    //获取视频流索引
    int videoIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, 0, 0);
    if (videoIndex < 0) {
        LOGE("av_find_best_stream video is empty");
        mutex.unlock();
        return XParameter();
    }
    this->videoStreamIndex = videoIndex;

    XParameter para;
    AVStream *pStream = formatContext->streams[videoIndex];
    para.parameters = pStream->codecpar;
    mutex.unlock();
    return para;
}

XParameter FFDemux::getAudioParameter() {
    mutex.lock();
    if (!formatContext) {
        LOGE("get audio param failed ic is null");
        mutex.unlock();
        return XParameter();
    }
    //获取音频流索引
    int audioIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (audioIndex < 0) {
        LOGE("av_find_best_stream audio is empty");
        mutex.unlock();
        return XParameter();
    }
    this->audioStreamIndex = audioIndex;

    XParameter para;
    para.parameters = formatContext->streams[audioIndex]->codecpar;
    para.channels = formatContext->streams[audioIndex]->codecpar->channels;
    para.sampleRate = formatContext->streams[audioIndex]->codecpar->sample_rate;
    mutex.unlock();

    return para;
}

AVStream *FFDemux::getAudioStream() {
    return formatContext->streams[audioStreamIndex];
}

AVStream *FFDemux::getVideoStream() {
    return formatContext->streams[videoStreamIndex];

}



void FFDemux::close() {
    mutex.lock();
    if (formatContext)
        avformat_close_input(&formatContext);
    mutex.unlock();
}

FFDemux::~FFDemux() {
    delete formatContext;
    LOGI("~FFDemux");
}

