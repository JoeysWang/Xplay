//
// Created by 王越 on 2020/8/9.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H

#include <thread>
#include <memory>
#include "../data/PlayerState.h"
#include "../data/FrameData.h"
#include "../data/DecodeParam.h"
#include "../dataStructure/Queue.h"

typedef void (*AudioPCMCallback)(double pts, uint8_t *stream, int len, void *context);

class IAudioPlay {
public:
    IAudioPlay(const std::shared_ptr<PlayerState> &playerState);

    virtual ~IAudioPlay();

    //缓冲满后会阻塞
    void update(FrameData data);

    virtual bool startPlay(DecodeParam out) = 0;

    //获取缓冲数据，如没有就阻塞
    FrameData getData();

public:
    int maxFrameBuffer = 100;
    int pts;

protected:
    std::mutex mutex;
    std::unique_ptr<Queue<FrameData>> frames;

    AudioPCMCallback callback;  // 音频回调
    void *callbackContext = nullptr;
    bool isExit = false;
    std::shared_ptr<PlayerState> playerState;

public:
    void setCallback(AudioPCMCallback pcmCallback, void *context);

    virtual void quit();
};


#endif //XPLAY_IAUDIOPLAY_H