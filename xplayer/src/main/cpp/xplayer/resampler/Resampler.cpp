//
// Created by 王越 on 2020/8/9.
//

#include <XLog.h>
#include "Resampler.h"
#include "../audio/SLAudioPlayer.h"

Resampler::Resampler(const std::shared_ptr<PlayerState> &playerState) : playerState(playerState) {
    audioPlayer = std::make_unique<SLAudioPlayer>(playerState);
}

bool Resampler::open(DecodeParam in, DecodeParam out) {
    //音频重采样初始化
    swrContext = swr_alloc();
    //输入输出参数
    outChannels = in.parameters->channels;
    outFormat = AV_SAMPLE_FMT_S16;

    LOGD("FFResample params:\nout_ch_layout=%d\nout_sample_fmt=%d\nout_sample_rate=%d\n",
         outChannels, outFormat, out.parameters->sample_rate);
    LOGD("FFResample params:\nin_ch_layout=%d\nin_sample_fmt=%d\nin_sample_rate=%d\n",
         in.parameters->channels, in.parameters->format, in.parameters->sample_rate);

    swrContext = swr_alloc_set_opts(
            swrContext,
            av_get_default_channel_layout(outChannels),//2声道输出
            (AVSampleFormat) outFormat,//写死
            out.parameters->sample_rate,
            av_get_default_channel_layout(in.parameters->channels),//原通道数
            (AVSampleFormat) in.parameters->format,
            in.parameters->sample_rate,
            0,
            0
    );
    int re = swr_init(swrContext);
    if (re != 0) {
        LOGW("音频重采样初始化 swr_init 失败 %s", av_err2str(re));
        return false;
    }

    LOGW("音频重采样初始化成功");
    audioPlayer->startPlay(out);
    return true;
}

FrameData Resampler::resample(FrameData *in) {

    //输出空间分配
    FrameData out;
    //通道数 * 单通道样本数 * 样本字节大小

    if (!swrContext) {
        LOGE("!swrContext");
        return out;
    }

    int size = outChannels * in->nb_samples * av_get_bytes_per_sample((AVSampleFormat) outFormat);
    if (!size) {
        LOGE("!size");
        return out;
    }
    delete in;
    out.allocResampleData(size, 0);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.resampleData;
    int len = swr_convert(swrContext,
                          outArr,
                          in->nb_samples,
                          (const uint8_t **) in->decodeDatas,
                          in->nb_samples
    );

    if (len <= 0) {
        out.release();
        LOGE("音频重采样 swr_convert 失败  ");
        return out;
    }
//    LOGI("音频重采样 swr_convert 成功  size=%d", size);
    out.size = size;
    out.pts = in->pts;
    return out;
}

void Resampler::update(FrameData *data) {
    FrameData out = resample(data);
    if (out.size > 0) {
        audioPlayer->update(out);
    }
}

void Resampler::quit() {
    audioPlayer->quit();
    if (swrContext) {
        swr_free(&swrContext);
        swrContext = nullptr;
    }
}
