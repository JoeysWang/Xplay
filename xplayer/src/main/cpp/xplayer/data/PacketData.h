//
// Created by 王越 on 2020/8/7.
//

#ifndef FFPLAYER_PACKETDATA_H
#define FFPLAYER_PACKETDATA_H


extern "C" {
#include <libavformat/avformat.h>
};

class PacketData {
public:
    AVPacket *packet;
    int size;
    AVRational frame_rate;
    AVRational time_base;
    int mediaType;

public:
    void release() {
        av_packet_unref(packet);
        packet = nullptr;
        size = 0;
        delete this;
    };
};


#endif //FFPLAYER_PACKETDATA_H
