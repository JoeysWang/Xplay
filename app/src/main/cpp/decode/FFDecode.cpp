//
// Created by 王越 on 2020/4/29.
//

#include "FFDecode.h"
#include "../XLog.h"
#include "../data/XParameter.h"
#include "../data/XData.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

}

FFDecode::FFDecode() {
    frameQueue = new FrameQueue(3, 1);
    packetQueue = new PacketQueue();
}

void FFDecode::start() {
    LOGI("FFDecode::start");
    frameQueue->start();
    packetQueue->start();

    XThread::start();
}

FFDecode::~FFDecode() {
    mutex.lock();
    if (packetQueue) {
        packetQueue->flush();
        delete packetQueue;
        packetQueue = NULL;
    }
    if (frameQueue) {
        frameQueue->flush();
        delete frameQueue;
        frameQueue = NULL;
    }
    mutex.unlock();
}

bool FFDecode::open(XParameter parameter, bool isHard) {
    AVCodecParameters *parameters = parameter.parameters;
    if (!parameters)return false;

    //1.查找解码器
    AVCodec *avCodec = avcodec_find_decoder(parameters->codec_id);
    LOGI("avcodec_find_decoder video codec_id=%d ", parameters->codec_id);
    if (isHard) {
        avCodec = avcodec_find_decoder_by_name("h264_mediacodec");
    }

    if (!avCodec) {
        LOGE("avcodec_find_decoder error %d ", parameters->codec_id);
        return false;
    }
    LOGI("avcodec_find_decoder success %d! ", isHard);

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
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO)
        audioOrVideo = 1;
    else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO)
        audioOrVideo = 0;

    return true;
}

bool FFDecode::sendPacket(XData pkt) {
    if (!codecContext) {
        return false;
    }
    if (!pkt.data) {
        return false;
    }

    codecContext->thread_count = 8;
    int re = avcodec_send_packet(codecContext, (AVPacket *) pkt.data);
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
    while (true) {
        if (!avFrame) {
            avFrame = av_frame_alloc();
        }
        int re = avcodec_receive_frame(codecContext, avFrame);
        if (re < 0) {
            if (re == AVERROR_EOF) {
                avcodec_flush_buffers(codecContext);
                return XData();
            } else if (re == AVERROR(EAGAIN)) {
//                LOGE("video avcodec_receive_frame(): output is not available in this state user must try to send new input");
                break;
            } else {
//                LOGE("video avcodec_receive_frame(): other errors\n");
                continue;
            }
        } else {
//        frame->pts = frame->best_effort_timestamp;
            //frame->pts = frame->pkt_dts;

            // 成功解码得到一个视频帧或一个音频帧，则返回
            XData d;
            d.data = (unsigned char *) (avFrame);
            if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
                d.size = (avFrame->linesize[0] +
                          avFrame->linesize[1] +
                          avFrame->linesize[2]) * avFrame->height;

                d.width = avFrame->width;
                d.height = avFrame->height;

                d.linesize[0] = avFrame->linesize[0];
                d.linesize[1] = avFrame->linesize[1];
                d.linesize[2] = avFrame->linesize[2];
            } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
                //样本大小 * 单通道样本数 * 通道数
                d.size = av_get_bytes_per_sample(static_cast<AVSampleFormat>(avFrame->format))
                         * avFrame->nb_samples
                         * 2;
            }
            d.format = avFrame->format;
            d.pts = avFrame->best_effort_timestamp;

            memcpy(d.datas, avFrame->data, sizeof(d.datas));
            return d;
        }
    }


    return XData();
}
