//
// Created by 王越 on 2020/8/6.
//

#include <thread>
#include "Demuxer.h"
#include "XLog.h"
#include "ThreadUtils.h"


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
    int re = avformat_open_input(&formatContext, url, 0, 0);
    if (re != 0) {
        char buf[1024] = {0};
//        av_strerror(re, buf, sizeof(buf));
        LOGE("open ffdemux failed %s", url);
        return;
    }
    getAllStream();
    re = avformat_find_stream_info(formatContext, 0);
    if (re != 0) {
        char buf[1024] = {0};
//        av_strerror(re, buf, sizeof(buf));
        LOGE("avformat_find_stream_info failed %s", url);
        return;
    }
}

AVStream *Demuxer::getAudioStream() {
    return nullptr;
}

AVStream *Demuxer::getVideoStream() {
    return nullptr;
}

DecodeParam Demuxer::getVideoParameter() {
    return DecodeParam();
}

DecodeParam Demuxer::getAudioParameter() {
    return DecodeParam();
}

/**
 * run on New Thread!
 */
void Demuxer::readPacket() {
    while (true) {
        if (playerState->pauseRequest) {
            ThreadUtils::sleep(200);
            continue;
        }
        if (playerState->abortRequest) {
            break;
        }


    }
}

Demuxer::~Demuxer() {
//    avformat_network_deinit();

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