//
// Created by ky611 on 2020/6/14.
//

#include "FFResample.h"
#include "../XLog.h"

bool FFResample::open(XParameter in, XParameter out) {
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

    return true;
}

XData FFResample::resample(XData in) {

    //输出空间分配
    XData out;
    //通道数 * 单通道样本数 * 样本字节大小

    if (!swrContext) {
        LOGE("!swrContext");
        return XData();
    }

    int size =
            outChannels * in.nb_samples * av_get_bytes_per_sample((AVSampleFormat) outFormat);
    if (!size) {
        LOGE("!size");
        return XData();
    }
    out.alloc(size);
    uint8_t *outArr[2] = {0};
    outArr[0] = out.resampleData;
    int len = swr_convert(swrContext,
                          outArr,
                          in.nb_samples,
                          (const uint8_t **) in.decodeDatas,
                          in.nb_samples
    );
//    for (int i = 0; i < 2; ++i) {
//        LOGD("outArr[%d]=%p size=%ld",i,outArr[i], sizeof(outArr[i]));
//        if (i==1)
//            LOGD("------");
//    }

    if (len <= 0) {
        out.drop();
        LOGE("音频重采样 swr_convert 失败  ");
        return XData();
    }
    out.size = size;
    out.pts = in.pts;
    return out;
}
