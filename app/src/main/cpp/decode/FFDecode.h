//
// Created by 王越 on 2020/4/29.
//

#ifndef XPLAY_FFDECODE_H
#define XPLAY_FFDECODE_H


#include "IDecode.h"
#include <mutex>

struct AVCodecContext;
struct AVFrame;

class FFDecode : public IDecode {
public:


    FFDecode();

    bool open(XParameter parameter, bool isHard) override;

    bool sendPacket(XData pkt) override;

    XData receiveFrame() override;

    virtual ~FFDecode();

    void start() override;

    void close();

protected:

    std::mutex mutex;

    AVCodecContext *codecContext = 0;
    AVFrame *avFrame = 0;
    AVPacket flush_pkt;

};


#endif //XPLAY_FFDECODE_H
