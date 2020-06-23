//
// Created by ky611 on 2020/6/14.
//

#include "FFResample.h"
#include "../XLog.h"

bool FFResample::open(XParameter in, XParameter out) {
    //音频重采样初始化
    swrContext = swr_alloc();
    //输入输出参数
    swrContext = swr_alloc_set_opts(
            swrContext,
            av_get_default_channel_layout(out.channels),//2声道输出
            AV_SAMPLE_FMT_S16,//写死
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
    outChannels = in.parameters->channels;
    outFormat = AV_SAMPLE_FMT_S16;

    return true;
}

XData FFResample::resample(XData in) {

    //输出空间分配
    XData out;

    auto *frame = (AVFrame *) in.data;
    //通道数 * 单通道样本数 * 样本字节大小

    if (!swrContext)
        return XData();

    if (in.size <= 0 || !in.data) {
        return XData();
    }
    int size =
            outChannels * frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat) outFormat);
//    LOGD("resample size = %d", size);
    if (!size) {
        return XData();
    }
    out.alloc(size);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.data;
    int len = swr_convert(swrContext,
                          outArr,
                          frame->nb_samples,
                          (const uint8_t **) frame->data,
                          frame->nb_samples
    );

    if (len <= 0) {
        out.drop();
        LOGE("音频重采样 swr_convert 失败  ");
        return XData();
    }

    out.pts = in.pts;

    return out;
}
