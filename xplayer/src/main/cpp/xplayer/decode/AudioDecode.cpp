//
// Created by 王越 on 2020/8/6.
//

#include <XLog.h>
#include "AudioDecode.h"
#include <thread>

extern "C" {
#include <libavutil/frame.h>
}

AudioDecode::AudioDecode(const std::shared_ptr<PlayerState> &playerState) : IDecode(playerState) {
    LOGI("AudioDecode::AudioDecode");
    tag = "audio";
    packetQueue->tag = "audio";
    frameQueue->tag = "audioFrame";
}

int AudioDecode::decode() {
    int ret = 0;
    while (!isExit) {
        isLooping = true;
        if (isExit || playerState->abortRequest) {
            ret = -1;
            break;
        }
        if (playerState->pauseRequest) {
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            mutex.unlock();
            continue;
        }
        PacketData *input;
        if (!packetQueue->pop(input)) {
            ret = -1;
            break;
        }

        AVPacket *pkt = input->packet;
        ret = avcodec_send_packet(codecContext, pkt);
        if (ret < 0) {
            mutex.unlock();
            // 一次解码无法消耗完AVPacket中的所有数据，需要重新解码
            LOGD("一次解码无法消耗完AVPacket中的所有数据，需要重新解码");
            if (ret == AVERROR(EAGAIN)) {
                continue;
            } else {
                delete input;
            }
            continue;
        }
        av_packet_unref(pkt);
        delete input;

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);

        if (ret < 0) {
            av_frame_unref(frame);
            LOGE("audio avcodec_receive_frame error %s", av_err2str(ret));
            continue;
        } else {
            // 这里要重新计算frame的pts 否则会导致网络视频出现pts 对不上的情况
            AVRational tb = (AVRational) {1, frame->sample_rate};

            auto *output = new FrameData;

            output->size = av_get_bytes_per_sample(
                    static_cast<AVSampleFormat>(frame->format))
                           * frame->nb_samples
                           * 2;
            output->format = frame->format;
            output->nb_samples = frame->nb_samples;
            LOGI("audio frame size=%d  \n=====", output->size);
            output->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            memcpy(output->decodeDatas, frame->data, sizeof(frame->data));

            av_frame_unref(frame);

            frameQueue->push(output);
        }

    }
    isLooping = false;
    loopingSignal.notify_all();
    return ret;
}

