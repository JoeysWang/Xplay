//
// Created by 王越 on 2020/8/6.
//

#include <XLog.h>
#include "AudioDecode.h"
#include <thread>

AudioDecode::AudioDecode(const std::shared_ptr<PlayerState> &playerState) : IDecode(playerState) {
    LOGI("AudioDecode::AudioDecode");
    packetQueue->tag = "audio";
}

int AudioDecode::decode() {
    int got_frame = 0;
    int ret = 0;
    AVFrame *frame;
    for (;;) {
        if (isExit || playerState->abortRequest) {
            ret = -1;
            break;
        }
        if (playerState->pauseRequest) {
            got_frame = 0;
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        PacketData *input;
        if (!packetQueue->pop(input)) {
            ret = -1;
            break;
        }

        AVPacket *pkt = input->packet;

        // 将数据包解码
        ret = avcodec_send_packet(codecContext, pkt);
        if (ret < 0) {
            // 一次解码无法消耗完AVPacket中的所有数据，需要重新解码
            LOGD("一次解码无法消耗完AVPacket中的所有数据，需要重新解码");
            if (ret == AVERROR(EAGAIN)) {
                continue;
            } else {
                delete input;
            }
            continue;
        }
        delete input;
        frame = av_frame_alloc();
        // 获取解码得到的音频帧AVFrame
        ret = avcodec_receive_frame(codecContext, frame);
        // 释放数据包的引用，防止内存泄漏
        av_packet_unref(pkt);
        if (ret < 0) {
            av_frame_unref(frame);
            LOGE("audio avcodec_receive_frame error %s", av_err2str(ret));
            got_frame = 0;
            continue;
        } else {
            got_frame = 1;
            // 这里要重新计算frame的pts 否则会导致网络视频出现pts 对不上的情况
            AVRational tb = (AVRational) {1, frame->sample_rate};

            FrameData *output;
            if (!(output = frameQueue->peekWritable())) {
                ret = -1;
                break;
            }
            output->size = av_get_bytes_per_sample(
                    static_cast<AVSampleFormat>(frame->format))
                           * frame->nb_samples
                           * 2;
            output->format = frame->format;
            output->nb_samples = frame->nb_samples;
            LOGI("audio frame size=%d  \n=====", output->size);
            output->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);

            memcpy(output->decodeDatas, frame->data, sizeof(frame->data));

            frameQueue->pushFrame();
        }

    }
    return ret;
}

