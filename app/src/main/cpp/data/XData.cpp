//
// Created by 王越 on 2020/4/26.
//
extern "C" {

#include <libavcodec/avcodec.h>
}

#include "XData.h"

void XData::drop() {
    if (!data)return;
    if (type == AVPACKET_TYPE) {
        av_packet_free((AVPacket **) &data);
    } else
        delete data;
    data = 0;
    size = 0;
}

bool XData::alloc(int size, const char *data) {
    drop();
    type = UCHAR_TYPE;
    if (size <= 0) { return false; }

    this->data = new unsigned char[size];
    if (!this->data) { return false; }
    if (data) {
        memcpy(this->data, data, size);
    }
    return true;
}
