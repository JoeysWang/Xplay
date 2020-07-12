//
// Created by 王越 on 2020/4/26.
//
extern "C" {

#include <libavcodec/avcodec.h>
}

#include "XData.h"

void XData::drop() {
    if (allocType == AVPACKET_TYPE) {
        av_packet_free(&packet);
    } else {
        av_frame_unref(frame);
        delete[] resampleData;
        resampleData = nullptr;
    }
    size = 0;
}

bool XData::alloc(int size, const char *data) {
    drop();
    allocType = AVFRAME_TYPE;
    if (size <= 0) { return false; }

    this->resampleData = new unsigned char[size];
    if (!this->resampleData) { return false; }
    if (data) {
        memcpy(this->resampleData, data, size);
    }
    this->size = size;
    return true;
}
