//
// Created by 王越 on 2020/4/26.
//
extern "C" {

#include <libavcodec/avcodec.h>
}

#include "XData.h"

void XData::Drop() {
    if (!data)return;
    av_packet_free((AVPacket **) &data);
    data = 0;
    size = 0;
}
