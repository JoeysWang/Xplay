//
// Created by 王越 on 2020/8/6.
//

#include <XLog.h>
#include "VideoDecode.h"
#include "../data/FrameData.h"
#include <thread>

VideoDecode::VideoDecode(const std::shared_ptr<PlayerState> &playerState) : IDecode(playerState) {
    LOGI("VideoDecode::VideoDecode");
    tag = "Video";
    packetQueue->tag = "Video";
    frameQueue->tag = "VideoFrame";
}

int VideoDecode::decode() {
    LOGI(" VideoDecode::decode");
    int ret = 0;
    AVRational tb = stream->time_base;
    AVRational frame_rate = av_guess_frame_rate(formatContext, stream, NULL);
    while (!isExit) {
        isLooping = true;

        if (isExit || playerState->abortRequest || !codecContext) {
            ret = -1;
            break;
        }
        if (playerState->pauseRequest) { ;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            mutex.unlock();
            continue;
        }

        PacketData *input;
        if (!packetQueue->pop(input)) {
            ret = -1;
            mutex.unlock();
            break;
        }
        mutex.lock();
        AVPacket *packet = input->packet;
        ret = avcodec_send_packet(codecContext, packet);

        if (ret < 0) {
            av_packet_unref(packet);
            delete input;
            LOGE("videodecode avcodec_send_packet %s", av_err2str(ret));
            mutex.unlock();
            continue;
        }

        av_packet_unref(packet);
        delete input;
        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);
        if (ret < 0) {
            LOGE("video avcodec_receive_frame error %s", av_err2str(ret));
            av_frame_unref(frame);
            av_packet_unref(packet);
            mutex.unlock();

            continue;
        } else {
            frame->pts = frame->best_effort_timestamp;
            frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(formatContext, stream,
                                                                      frame);
            // 取出帧

            auto *output = new FrameData;
            output->linesize[0] = frame->linesize[0];
            output->linesize[1] = frame->linesize[1];
            output->linesize[2] = frame->linesize[2];
            output->frameWidth = frame->width;
            output->frameHeight = frame->height;
            output->format = frame->format;
            output->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            output->duration = frame_rate.num && frame_rate.den
                               ? av_q2d((AVRational) {frame_rate.den, frame_rate.num}) : 0;
            output->size = (frame->linesize[0] +
                            frame->linesize[1] +
                            frame->linesize[2]) * frame->height;
//            LOGI("video frame size=%d  \n=====", output->size);
            memcpy(output->decodeDatas, frame->data, sizeof(frame->data));
            frameQueue->push(output);
            mutex.unlock();

        }
    }
    isLooping = false;
    loopingSignal.notify_all();
    return ret;
}
