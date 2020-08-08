//
// Created by 王越 on 2020/8/6.
//

#include <XLog.h>
#include "VideoDecode.h"
#include "../data/FrameData.h"
#include <thread>

VideoDecode::VideoDecode() {
    LOGI("VideoDecode::VideoDecode");
    packetQueue->tag = "Video";
}

void VideoDecode::decode() {
    LOGI(" VideoDecode::decode");
    AVFrame *frame;
    FrameData *output;

    int got_picture;
    int ret = 0;
    AVPacket *packet;
    AVRational tb = stream->time_base;
    AVRational frame_rate = av_guess_frame_rate(formatContext, stream, NULL);
    for (;;) {
        if (isExit || playerState->abortRequest) {
            ret = -1;
            break;
        }
        if (playerState->pauseRequest) { ;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
        PacketData input;
        if (!packetQueue->pop(input)) {
            ret = -1;
            break;
        }
        packet = input.packet;
        // 送去解码
        ret = avcodec_send_packet(codecContext, packet);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            av_packet_unref(packet);
            input.release();
            LOGE("videodecode avcodec_send_packet %s", av_err2str(ret));
            continue;
        }
        input.release();
        // 得到解码帧
        frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);
        if (ret < 0 && ret != AVERROR_EOF) {
            LOGE("video avcodec_receive_frame error %s", av_err2str(ret));
            av_frame_unref(frame);
            av_packet_unref(packet);
            continue;
        } else {
            got_picture = 1;
            frame->pts = frame->best_effort_timestamp;
            frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(formatContext, stream,
                                                                      frame);
        }
        if (got_picture) {
            // 取出帧
            if (!(output = frameQueue->peekWritable())) {
                ret = -1;
                break;
            }
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
            memcpy(output->decodeDatas, frame->data, sizeof(frame->data));
            av_frame_unref(output->frame);
            av_frame_move_ref(output->frame, frame);
            frameQueue->pushFrame();
        }
        av_frame_unref(frame);
        av_packet_unref(packet);
    }

    if (frame) {
        av_frame_free(&frame);
        av_free(frame);
        frame = nullptr;
    }
    if (packet) {
        av_packet_free(&packet);
        av_free(packet);
        packet = nullptr;
    }
}
