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
    AVPacket *packet = nullptr;
    int size = 0;
    AVRational frame_rate = {1, 1};
    AVRational time_base = {1, 1};
    int mediaType = 0;

public:
    void release() {
        av_packet_unref(packet);
        packet = nullptr;
        size = 0;
    };
};


#endif //FFPLAYER_PACKETDATA_H
