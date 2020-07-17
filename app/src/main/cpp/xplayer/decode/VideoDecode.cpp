//
// Created by 王越 on 2020/4/29.
//

#include <thread>
#include "VideoDecode.h"
#include "../../XLog.h"
#include "../data/XParameter.h"
#include "../data/XData.h"

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

VideoDecode::VideoDecode(PlayerState *playerState) : IDecode(playerState) {
    frameQueue = new FrameQueue(FRAME_QUEUE_SIZE, 1);
    packetQueue = new Queue<XData>(100);
}

void VideoDecode::start() {
    LOGI("VideoDecode::start");
    frameQueue->start();
    XThread::start();
}

int VideoDecode::decodePacket() {
    AVFrame *frame;
    XData *output;

    int got_picture;
    int ret = 0;
    AVPacket *packet;
    AVRational tb = pStream->time_base;
    AVRational frame_rate = av_guess_frame_rate(formatCtx, pStream, NULL);
    for (;;) {
        if (isExit || playerState->abortRequest) {
            ret =-1;
            break;
        }
        if (playerState->pauseRequest) {
//            LOGI("VideoDecode sleep for pause");
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        XData input;
        if (!packetQueue->pop(input)) {
            ret = -1;
            break;
        }
        packet = input.packet;
        // 送去解码
        ret = avcodec_send_packet(codecContext, packet);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            av_packet_unref(packet);
            input.drop();
            LOGE("videodecode avcodec_send_packet %s", av_err2str(ret));
            continue;
        }
        input.drop();
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
//            if (masterClock != nullptr) {
//                double dpts = NAN;
//
//                if (frame->pts != AV_NOPTS_VALUE) {
//                    dpts = av_q2d(pStream->time_base) * frame->pts;
//                }
//                // 计算视频帧的长宽比
//                frame->sample_aspect_ratio = av_guess_sample_aspect_ratio(formatCtx, pStream,
//                                                                          frame);
//                // 是否需要做舍帧操作
//                if (playerState->frameDrop > 0 ||
//                    (playerState->frameDrop > 0 && playerState->syncType != AV_SYNC_VIDEO)) {
//                    if (frame->pts != AV_NOPTS_VALUE) {
//                        double diff = dpts - masterClock->getClock();
//                        if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
//                            diff < 0 && packetQueue->length() > 0) {
//                            av_frame_unref(frame);
//                            got_picture = 0;
//                        }
//                    }
//                }
//            }
        }
        if (got_picture) {
            // 取出帧
            if (!(output = frameQueue->peekWritable())) {
                ret = -1;
                break;
            }
            output->allocType=AVFRAME_TYPE;
            output->linesize[0] = frame->linesize[0];
            output->linesize[1] = frame->linesize[1];
            output->linesize[2] = frame->linesize[2];
            output->width = frame->width;
            output->height = frame->height;
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
    return ret;
}

void VideoDecode::close() {
    mutex.lock();

    mutex.unlock();
}
