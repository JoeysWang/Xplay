//
// Created by 王越 on 2020/4/26.
//
#include <jni.h>
#include "FFDemux.h"
#include "../XLog.h"
#include "../data/XParameter.h"
#include "../decode/IDecode.h"

extern "C" {
#include <libavformat/avformat.h>
}

FFDemux::FFDemux(PlayerState *playerState) : IDemux(playerState) {

    //注册所有解封装器
    formatContext = nullptr;
    av_register_all();
    avcodec_register_all();
    avformat_network_init();
    LOGI("regsist ffmpeg");
}

FFDemux::~FFDemux() {
    avformat_network_deinit();
    LOGI("~FFDemux");
}

void FFDemux::getAllStream() {
    if (!formatContext) { return; }

    for (int i = 0; i < formatContext->nb_streams; i++) {
        AVStream *in_stream = formatContext->streams[i];

        switch (in_stream->codec->codec_type) {
            case AVMEDIA_TYPE_UNKNOWN: {
                LOGI("stream AVMEDIA_TYPE_UNKNOWN index=%d\n", in_stream->codec->codec_id);
                break;
            }
            case AVMEDIA_TYPE_VIDEO: {
                LOGI("stream AVMEDIA_TYPE_VIDEO index=%d\n", in_stream->codec->codec_id);
                break;
            }
            case AVMEDIA_TYPE_AUDIO: {
                LOGI("stream AVMEDIA_TYPE_AUDIO index=%d\n", in_stream->codec->codec_id);
                break;
            }
            case AVMEDIA_TYPE_DATA: {
                LOGI("stream AVMEDIA_TYPE_DATA index=%d\n", in_stream->codec->codec_id);
                break;
            }
            case AVMEDIA_TYPE_SUBTITLE: {
                LOGI("stream AVMEDIA_TYPE_SUBTITLE index=%d\n", in_stream->codec->codec_id);
                break;
            }
            case AVMEDIA_TYPE_ATTACHMENT: {
                LOGI("stream AVMEDIA_TYPE_ATTACHMENT index=%d\n", in_stream->codec->codec_id);
                break;
            }
            case AVMEDIA_TYPE_NB: {
                LOGI("stream AVMEDIA_TYPE_NB index=%d\n", in_stream->codec->codec_id);
                break;
            }
        }

    }

}

//打开文件、流媒体 http rtsp
bool FFDemux::open(const char *url) {
    close();
    mutex.lock();
    LOGD("FFDemux::open %s", url);
    int re = avformat_open_input(&formatContext, url, 0, 0);
    if (re != 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf));
        LOGE("open ffdemux failed %s", url);
        mutex.unlock();
        return false;
    }
    getAllStream();
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

void FFDemux::readPacket() {
    AVPacket *avPacket;
    XData *output;
    for (;;) {
        mutex.lock();
        if (playerState->abortRequest || isExit) {
            mutex.unlock();
            return;
        }
        if (playerState->pauseRequest) {
            XSleep(50);
            LOGI("demux sleep for pause");
            mutex.unlock();
            continue;
        }
        avPacket = av_packet_alloc();
        int ret = av_read_frame(formatContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            if (ret == AVERROR_EOF) {
                LOGI("demux read 到结尾");
                stop();
                mutex.unlock();
                return;
            }
            mutex.unlock();
            continue;
        }
        output = new XData();
        output->allocType = AVPACKET_TYPE;
        output->packet = avPacket;
        output->size = avPacket->size;
        if (!pStream)
            pStream = formatContext->streams[avPacket->stream_index];

        AVRational frame_rate = av_guess_frame_rate(formatContext,
                                                    pStream,
                                                    NULL);
        output->frame_rate = frame_rate;
        output->time_base = pStream->time_base;
        if (avPacket->stream_index == audioStreamIndex) {
            output->audioOrVideo = MEDIA_TYPE_AUDIO;
            if (audioDecode)audioDecode->pushPacket(output);
        } else if (avPacket->stream_index == videoStreamIndex) {
            output->audioOrVideo = MEDIA_TYPE_VIDEO;
            if (videoDecode)videoDecode->pushPacket(output);
        }
        mutex.unlock();
    }

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

