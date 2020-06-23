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
    videoQueue = new Queue<XData>(100);
    audioQueue = new Queue<XData>(100);
}

void FFDecode::start() {
    LOGI("FFDecode::start");
    frameQueue->start();

    XThread::start();
}

FFDecode::~FFDecode() {
    mutex.lock();
    if (audioQueue) {
        audioQueue->quit();
        delete audioQueue;
        audioQueue = NULL;
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
        audioOrVideo = MEDIA_TYPE_VIDEO;
    else if (codecContext->codec_type == AVMEDIA_TYPE_AUDIO)
        audioOrVideo = MEDIA_TYPE_AUDIO;

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
        av_packet_unref((AVPacket *) pkt.data);
        return false;
    }

    return true;
}

XData FFDecode::receiveFrame() {
    if (!codecContext) {
        return XData();
    }
    XData *vp;
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
                avcodec_flush_buffers(codecContext);
                return XData();
            }
            if (ret == AVERROR(EAGAIN)) {
                return XData();
            }
            if (ret >= 0) {
//                LOGI("成功解码得到一个视频帧或一个音频帧，则返回 %d", audioOrVideo);
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
                    d.size = av_get_bytes_per_sample(
                            static_cast<AVSampleFormat>(avFrame->format))
                             * avFrame->nb_samples
                             * 2;
                }
                d.format = avFrame->format;
                d.pts = avFrame->best_effort_timestamp;
                memcpy(d.frameDatas, avFrame->data, sizeof(avFrame->data));
                av_frame_unref(avFrame);
                return d;
            }
        } while (ret != AVERROR(EAGAIN));
//        }
//        //初始化pkt_serial
//        do {
//            LOGI("初始化pkt_serial");
//            if (getPacketQueue()->length() == 0) {
//                /// packet_queue为空则等待
//                LOGI("  packet_queue为空则等待");
//                std::unique_lock<std::mutex> lock(mutex);
//                condition.wait(lock);
//            }
//
////            if (d->packet_pending) {        // 有未处理的packet则先处理
////                av_packet_move_ref(&pkt, &d->pkt);
////                d->packet_pending = 0;
////            } else {
//            /// 1. 取出一个packet。使用pkt对应的serial赋值给d->pkt_serial
//            LOGI(" 取出一个packet。使用pkt对应的serial赋值给d->pkt_serial");
//            XData data;
//            if (!getPacketQueue()->pop(data)) {
//                return data;
//            } else {
//                packet = (AVPacket *) data.data;
//                LOGI("getPacket success %ld", packet->duration);
//                pkt_serial = packet->duration;
//            }
////            }
//        } while (getPacketQueue()->serial != pkt_serial);

        /// packet_queue中第一个总是flush_pkt。每次seek操作会插入flush_pkt，更新serial，开启新的播放序列
//        if (packet->data == flush_pkt.data) {
//            /// 复位解码器内部状态/刷新内部缓冲区。当seek操作或切换流时应调用此函数。
//            //todo seek
//            avcodec_flush_buffers(codecContext);
////            d->finished = 0;
////            next_pts = start_pts;
////            next_pts_tb = start_pts_tb;
//        } else {
//            /// 2. 将packet发送给解码器
//            ///    发送packet的顺序是按dts递增的顺序，如IPBBPBB
//            ///    pkt.pos变量可以标识当前packet在视频文件中的地址偏移
//            LOGI("将packet发送给解码器");
//            if (avcodec_send_packet(codecContext,  packet) != 0) {
//                LOGE("avcodec_send_packet error.\n");
//                packet_pending = 1;
////                av_packet_move_ref(&d->pkt, &pkt);
//            } else
//                LOGI("avcodec_send_packet success.\n");
//        }


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
