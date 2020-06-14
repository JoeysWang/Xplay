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
            av_get_default_channel_layout(2),//2声道输出
            AV_SAMPLE_FMT_S16,//写死
            in.parameters->sample_rate,
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

    return false;
}

XData FFResample::resample(XData in) {
    return XData();
}
