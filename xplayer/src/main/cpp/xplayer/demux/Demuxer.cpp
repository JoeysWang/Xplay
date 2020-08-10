//
// Created by 王越 on 2020/8/6.
//

#include <thread>
#include "Demuxer.h"
#include "XLog.h"
#include "../../xhandler/ThreadUtils.h"
#include "../data/PacketData.h"
#include "../decode/IDecode.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
}

Demuxer::Demuxer(std::shared_ptr<PlayerState> const &state) {
    formatContext = nullptr;
    playerState = state;

    av_register_all();
    avformat_network_init();

    LOGI("Demuxer::Demuxer");

    std::thread th(&Demuxer::readPacket, this);
    th.detach();
}

void Demuxer::openSource(const char *url) {
    std::unique_lock<std::mutex> lock(mutex);
    int re = avformat_open_input(&formatContext, url, 0, 0);
    if (re != 0) {
        LOGE("open ffdemux failed %s", url);
        return;
    }

    re = avformat_find_stream_info(formatContext, 0);
    if (re != 0) {
        LOGE("avformat_find_stream_info failed %s", url);
        return;
    }
    LOGI("Demuxer::openSource success");
}

/**
 * run on New Thread!
 */
void Demuxer::readPacket() {
    AVPacket *avPacket;
    PacketData *output;
    isRunning = true;

    while (true) {
        if (!isRunning || !playerState || playerState->abortRequest) {
            break;
        }
        if (playerState->pauseRequest) {
            ThreadUtils::sleep(200);
            continue;
        }
        mutex.lock();
        avPacket = av_packet_alloc();
        int ret = av_read_frame(formatContext, avPacket);
        if (ret != 0) {
            av_packet_free(&avPacket);
            if (ret == AVERROR_EOF) {
                LOGI("demux read 到结尾");
                mutex.unlock();
                break;
            }
            mutex.unlock();
            continue;
        }

        output = new PacketData();
        output->packet = avPacket;
        output->size = avPacket->size;
        auto pStream = formatContext->streams[avPacket->stream_index];
        AVRational frame_rate = av_guess_frame_rate(formatContext, pStream, NULL);
        output->frame_rate = frame_rate;
        output->time_base = pStream->time_base;

        if (avPacket->stream_index == audioStreamIndex) {
            output->mediaType = MEDIA_TYPE_AUDIO;
            if (audioDecode)audioDecode->pushPacket(output);
        } else if (avPacket->stream_index == videoStreamIndex) {
            output->mediaType = MEDIA_TYPE_VIDEO;
            if (videoDecode)videoDecode->pushPacket(output);
        }
//        LOGI("FFDemux::readPacket size=%d", output->size);
        mutex.unlock();
    }
    isRunning = false;
    LOGI("Demuxer::readPacket  退出");
}

AVStream *Demuxer::getAudioStream() {
    return formatContext->streams[audioStreamIndex];
}

AVStream *Demuxer::getVideoStream() {
    return formatContext->streams[videoStreamIndex];
}

DecodeParam Demuxer::getVideoParameter() {
    if (!formatContext) {
        LOGE("get video param failed ic is null");
        return DecodeParam();
    }
    //获取视频流索引
    int videoIndex = av_find_best_stream(
            formatContext,
            AVMEDIA_TYPE_VIDEO,
            -1, -1, 0, 0);
    if (videoIndex < 0) {
        LOGE("av_find_best_stream video is empty");
        return DecodeParam();
    }
    this->videoStreamIndex = videoIndex;

    DecodeParam para;
    AVStream *pStream = formatContext->streams[videoIndex];
    para.parameters = pStream->codecpar;

    return para;
}

DecodeParam Demuxer::getAudioParameter() {
    if (!formatContext) {
        LOGE("get audio param failed ic is null");
        return DecodeParam();
    }
    //获取音频流索引
    int audioIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, 0, 0);
    if (audioIndex < 0) {
        LOGE("av_find_best_stream audio is empty");
        return DecodeParam();
    }
    this->audioStreamIndex = audioIndex;
    DecodeParam para;
    para.parameters = formatContext->streams[audioIndex]->codecpar;
    para.channels = formatContext->streams[audioIndex]->codecpar->channels;
    para.sampleRate = formatContext->streams[audioIndex]->codecpar->sample_rate;
    return para;
}

void Demuxer::quit() {
    LOGI("Demuxer::quit");
    avformat_network_deinit();
    if (isRunning) {
        isRunning = false;
    }
}

Demuxer::~Demuxer() {
    LOGI("Demuxer::~Demuxer");
    if (isRunning) {
        isRunning = false;
    }
}


void Demuxer::getAllStream() {
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