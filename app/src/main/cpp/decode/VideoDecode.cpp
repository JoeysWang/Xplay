//
// Created by 王越 on 2020/4/29.
//

#include <thread>
#include "VideoDecode.h"
#include "../XLog.h"
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

VideoDecode::~VideoDecode() {
    mutex.lock();
    LOGI("~VideoDecode");

    mutex.unlock();
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
        if (isExit) {
            ret = -1;
            break;
        }
        if (playerState->abortRequest) {
            return -1;
        }
        if (playerState->pauseRequest) {
            LOGI("VideoDecode sleep for pause");
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

            // 丢帧处理
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
            // 复制参数
//            vp->uploaded = 0;
            output->linesize[0] = frame->linesize[0];
            output->linesize[1] = frame->linesize[1];
            output->linesize[2] = frame->linesize[2];

            output->width = frame->width;
            output->height = frame->height;
            output->format = frame->format;
//            LOGI("video frame width=%d height=%d\n=====", output->width, output->height);
            output->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            output->duration = frame_rate.num && frame_rate.den
                               ? av_q2d((AVRational) {frame_rate.den, frame_rate.num}) : 0;
            output->size = (frame->linesize[0] +
                            frame->linesize[1] +
                            frame->linesize[2]) * frame->height;
            memcpy(output->decodeDatas, frame->data, sizeof(frame->data));
//            av_frame_move_ref(output->frame, frame);
            // 入队帧
            frameQueue->pushFrame();
        }

        // 释放数据包和缓冲帧的引用，防止内存泄漏
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


//bool FFDecode::sendPacket(XData pkt) {
//    if (!codecContext) {
//        return false;
//    }
//    if (!pkt.packet) {
//        return false;
//    }
//    codecContext->thread_count = 8;
//    int re = avcodec_send_packet(codecContext, pkt.packet);
//    if (re != 0) {
//        LOGE("avcodec_send_packet failed ");
//        av_packet_unref(pkt.packet);
//        return false;
//    }
//    stream_time_base = pkt.time_base;
//    return true;
//}
//
//XData FFDecode::receiveFrame() {
//    if (!codecContext) {
//        return XData();
//    }
//    int ret = AVERROR(EAGAIN);
//    for (;;) {
//        if (!avFrame) {
//            avFrame = av_frame_alloc();
//        }
//        // 本函数被各解码线程(音频、视频、字幕)首次调用时，d->pkt_serial等于-1，d->queue->serial等于1
////        if (pkt_serial == getPacketQueue()->serial) {
//        do {
//            if (getPacketQueue()->isFinish())
//                return XData();
//            switch (audioOrVideo) {
//                case MEDIA_TYPE_NONE:
//                    break;
//                case MEDIA_TYPE_VIDEO: {
//                    // 3.1 一个视频packet含一个视频frame
//                    //     解码器缓存一定数量的packet后，才有解码后的frame输出
//                    //     frame输出顺序是按pts的顺序，如IBBPBBP
//                    //     frame->pkt_pos变量是此frame对应的packet在视频文件中的偏移地址，值同pkt.pos
//                    ret = avcodec_receive_frame(codecContext, avFrame);
////                    if (ret >= 0) {
////                        //是否重排pts
////                        if (reorderVideoPts == -1) {
////                            avFrame->pts = avFrame->best_effort_timestamp;
////                        } else if (!reorderVideoPts) {
////                            avFrame->pts = avFrame->pkt_dts;
////                        }
////                    }
//                    break;
//                }
//                case MEDIA_TYPE_AUDIO: {
//                    // 3.2 一个音频packet含多个音频frame，每次avcodec_receive_frame()返回一个frame，此函数返回。
//                    // 下次进来此函数，继续获取一个frame，直到avcodec_receive_frame()返回AVERROR(EAGAIN)，
//                    // 表示解码器需要填入新的音频packet
//                    ret = avcodec_receive_frame(codecContext, avFrame);
//                    if (ret >= 0) {
////                        AVRational timeBase = AVRational{1, avFrame->sample_rate};
////                        if (avFrame->pts != AV_NOPTS_VALUE) {
////                            avFrame->pts = av_rescale_q(avFrame->pts,
////                                                        codecContext->pkt_timebase, timeBase);
////                        } else if (next_pts != AV_NOPTS_VALUE) {
////                            avFrame->pts = av_rescale_q(next_pts,
////                                                        next_pts_tb, timeBase);
////                        }
////                        if (avFrame->pts != AV_NOPTS_VALUE) {
////                            next_pts = avFrame->pts + avFrame->nb_samples;
////                            next_pts_tb = timeBase;
////                        }
//                    }
//                    break;
//                }
//            }
//
//            if (ret == AVERROR_EOF) {
//                LOGI("AVERROR AVERROR_EOF");
//                stop();
//                avcodec_flush_buffers(codecContext);
//                return XData();
//            }
//            if (ret == 0) {
//                // 成功解码得到一个视频帧或一个音频帧，则返回
//                XData *d;
//                if (!(d = frameQueue->peekWritable())) {
//                    LOGE("取出frame queue失败");
//                    break;
//                }
//                d->frame = avFrame;
//                if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
//                    d->size = (avFrame->linesize[0] +
//                               avFrame->linesize[1] +
//                               avFrame->linesize[2]) * avFrame->height;
//                    d->width = avFrame->width;
//                    d->height = avFrame->height;
//                    d->linesize[0] = avFrame->linesize[0];
//                    d->linesize[1] = avFrame->linesize[1];
//                    d->linesize[2] = avFrame->linesize[2];
//                    LOGI("video frame width=%d height=%d\n=====", d->width, d->height);
//
//                } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
//                    //样本大小 * 单通道样本数 * 通道数
//                    d->size = av_get_bytes_per_sample(
//                            static_cast<AVSampleFormat>(avFrame->format))
//                              * avFrame->nb_samples
//                              * 2;
//                    LOGI("audio frame sample_rate=%d\n=====", avFrame->sample_rate);
//                }
//
////                LOGI("codecContext 1/60 帧率的倒数 base=%d/%d", codecContext->time_base.num,
////                     codecContext->time_base.den);
//
//                d->format = avFrame->format;
//                d->pts = avFrame->pts;
//                d->time_base = codecContext->time_base;
//                memcpy(d->decodeDatas, avFrame->data, sizeof(avFrame->data));
//                frameQueue->pushFrame();
////                const char *type =
////                        (audioOrVideo == MEDIA_TYPE_VIDEO) ? "video" : "audio";
////                LOGE("取出%s frame queue成功handle=%d ,当前队列有%d个 rindex=%d,windex=%d",
////                     type,
////                     d,
////                     frameQueue->size,
////                     frameQueue->rindex,
////                     frameQueue->windex
////                );
////                av_frame_unref(avFrame);
//                return *d;
//            }
//        } while (ret != AVERROR(EAGAIN));
//
//    }
//}


void VideoDecode::close() {
    mutex.lock();

    mutex.unlock();
}
