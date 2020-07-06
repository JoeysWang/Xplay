//
// Created by 王越 on 2020/4/26.
//
extern "C" {

#include <libavcodec/avcodec.h>
}

#include "XData.h"

void XData::drop() {
    if (allocType == AVPACKET_TYPE) {
        av_packet_free((AVPacket **) &resampleData);
    } else
        delete resampleData;
    resampleData = 0;
    if (frame)
        av_frame_unref(frame);
    if (packet)
        av_packet_unref(packet);
    size = 0;
}

bool XData::alloc(int size, const char *data) {
    drop();
    allocType = UCHAR_TYPE;
    if (size <= 0) { return false; }

    this->resampleData = new unsigned char[size];
    if (!this->resampleData) { return false; }
    if (data) {
        memcpy(this->resampleData, data, size);
    }
    this->size = size;
    return true;
}
