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
    frameQueue = new FrameQueue(FRAME_QUEUE_SIZE, 1);
    packetQueue = new Queue<XData>(100);
}

void FFDecode::start() {
    LOGI("FFDecode::start");
    frameQueue->start();

    XThread::start();
}

FFDecode::~FFDecode() {
    mutex.lock();
    if (packetQueue) {
        packetQueue->quit();
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
    mutex.lock();
    codecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(codecContext, parameters);

    //3.打开解码器
    int re = avcodec_open2(codecContext, 0, 0);
    if (re < 0) {
        char buf[1024] = {0};
        av_strerror(re, buf, sizeof(buf) - 1);
        LOGE("avcodec_open2 error %s ", buf);
        mutex.unlock();
        return false;
    }
    if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO)
        audioOrVideo = MEDIA_TYPE_VIDEO;
    else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO)
        audioOrVideo = MEDIA_TYPE_AUDIO;
    mutex.unlock();

    return true;
}

bool FFDecode::sendPacket(XData pkt) {
    if (!codecContext) {
        return false;
    }
    if (!pkt.packet) {
        return false;
    }
    codecContext->thread_count = 8;
    int re = avcodec_send_packet(codecContext, pkt.packet);
    if (re != 0) {
        LOGE("avcodec_send_packet failed ");
        av_packet_unref(pkt.packet);
        return false;
    }

    return true;
}

XData FFDecode::receiveFrame() {
    if (!codecContext) {
        return XData();
    }
    int ret = AVERROR(EAGAIN);
    for (;;) {
        if (!avFrame) {
            avFrame = av_frame_alloc();
        }
        // 本函数被各解码线程(音频、视频、字幕)首次调用时，d->pkt_serial等于-1，d->queue->serial等于1
//        if (pkt_serial == getPacketQueue()->serial) {
        do {
            if (getPacketQueue()->isFinish())
                return XData();
            switch (audioOrVideo) {
                case MEDIA_TYPE_NONE:
                    break;
                case MEDIA_TYPE_VIDEO: {
                    // 3.1 一个视频packet含一个视频frame
                    //     解码器缓存一定数量的packet后，才有解码后的frame输出
                    //     frame输出顺序是按pts的顺序，如IBBPBBP
                    //     frame->pkt_pos变量是此frame对应的packet在视频文件中的偏移地址，值同pkt.pos
                    ret = avcodec_receive_frame(codecContext, avFrame);
                    if (ret >= 0) {
                        //是否重排pts
                        if (reorderVideoPts == -1) {
                            avFrame->pts = avFrame->best_effort_timestamp;
                        } else if (!reorderVideoPts) {
                            avFrame->pts = avFrame->pkt_dts;
                        }
                    }
                    break;
                }
                case MEDIA_TYPE_AUDIO: {
                    // 3.2 一个音频packet含多个音频frame，每次avcodec_receive_frame()返回一个frame，此函数返回。
                    // 下次进来此函数，继续获取一个frame，直到avcodec_receive_frame()返回AVERROR(EAGAIN)，
                    // 表示解码器需要填入新的音频packet
                    ret = avcodec_receive_frame(codecContext, avFrame);
                    if (ret >= 0) {
                        AVRational timeBase = AVRational{1, avFrame->sample_rate};
                        if (avFrame->pts != AV_NOPTS_VALUE) {
                            avFrame->pts = av_rescale_q(avFrame->pts,
                                                        codecContext->pkt_timebase, timeBase);
                        } else if (next_pts != AV_NOPTS_VALUE) {
                            avFrame->pts = av_rescale_q(next_pts,
                                                        next_pts_tb, timeBase);
                        }
                        if (avFrame->pts != AV_NOPTS_VALUE) {
                            next_pts = avFrame->pts + avFrame->nb_samples;
                            next_pts_tb = timeBase;
                        }
                    }
                    break;
                }
            }

            if (ret == AVERROR_EOF) {
                LOGI("AVERROR AVERROR_EOF");
                stop();
                avcodec_flush_buffers(codecContext);
                return XData();
            }
//            if (ret == AVERROR(EAGAIN)) {
//                LOGI("AVERROR，EAGAIN");
//                continue;
//            }
            if (ret == 0) {
                // 成功解码得到一个视频帧或一个音频帧，则返回
                XData *d;
                if (!(d = frameQueue->peekWritable())) {
                    LOGE("取出frame queue失败");
                    break;
                }
                d->frame = avFrame;
                if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
                    d->size = (avFrame->linesize[0] +
                               avFrame->linesize[1] +
                               avFrame->linesize[2]) * avFrame->height;
                    d->width = avFrame->width;
                    d->height = avFrame->height;
                    d->linesize[0] = avFrame->linesize[0];
                    d->linesize[1] = avFrame->linesize[1];
                    d->linesize[2] = avFrame->linesize[2];
                } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
                    //样本大小 * 单通道样本数 * 通道数
                    d->size = av_get_bytes_per_sample(
                            static_cast<AVSampleFormat>(avFrame->format))
                              * avFrame->nb_samples
                              * 2;
                }
                d->format = avFrame->format;
                d->pts = avFrame->pts;
                memcpy(d->decodeDatas, avFrame->data, sizeof(avFrame->data));
                frameQueue->pushFrame();
//                const char *type =
//                        (audioOrVideo == MEDIA_TYPE_VIDEO) ? "video" : "audio";
//                LOGE("取出%s frame queue成功handle=%d ,当前队列有%d个 rindex=%d,windex=%d",
//                     type,
//                     d,
//                     frameQueue->size,
//                     frameQueue->rindex,
//                     frameQueue->windex
//                );
//                av_frame_unref(avFrame);
                return *d;
            }
        } while (ret != AVERROR(EAGAIN));

    }
}


//XData FFDecode::receiveFrame() {
//    if (!codecContext) {
//        return XData();
//    }
//    while (true) {
//        if (!avFrame) {
//            avFrame = av_frame_alloc();
//        }
//        int re = avcodec_receive_frame(codecContext, avFrame);
//        if (re < 0) {
//            if (re == AVERROR_EOF) {
//                avcodec_flush_buffers(codecContext);
//                return XData();
//            } else if (re == AVERROR(EAGAIN)) {
////                LOGE("video avcodec_receive_frame(): output is not available in this state user must try to send new input");
//                break;
//            } else {
////                LOGE("video avcodec_receive_frame(): other errors\n");
//                continue;
//            }
//        } else {
//            /** 3.1 一个视频packet含一个视频frame
//              *  解码器缓存一定数量的packet后，才有解码后的frame输出
//              *  frame输出顺序是按pts的顺序，如IBBPBBP
//              * frame->pkt_pos变量是此frame对应的packet在视频文件中的偏移地址，值同pkt.pos
//            */
//            // 成功解码得到一个视频帧或一个音频帧，则返回
//            XData d;
//            d.data = (unsigned char *) (avFrame);
//            if (codecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
//                d.size = (avFrame->linesize[0] +
//                          avFrame->linesize[1] +
//                          avFrame->linesize[2]) * avFrame->height;
//
//                d.width = avFrame->width;
//                d.height = avFrame->height;
//
//                d.linesize[0] = avFrame->linesize[0];
//                d.linesize[1] = avFrame->linesize[1];
//                d.linesize[2] = avFrame->linesize[2];
//            } else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
//                //样本大小 * 单通道样本数 * 通道数
//                d.size = av_get_bytes_per_sample(static_cast<AVSampleFormat>(avFrame->format))
//                         * avFrame->nb_samples
//                         * 2;
//            }
//            d.format = avFrame->format;
//            d.pts = avFrame->best_effort_timestamp;
//
//            memcpy(d.datas, avFrame->data, sizeof(d.datas));
//            return d;
//        }
//    }
//
//
//    return XData();
//}
void FFDecode::close() {
    mutex.lock();

    mutex.unlock();
}
