//
// Created by ky611 on 2020/6/16.
//

#ifndef XPLAY_IAUDIOPLAY_H
#define XPLAY_IAUDIOPLAY_H


#include <list>
#include "../IObserver.h"
#include "../data/XParameter.h"
#include "../player/PlayerState.h"

typedef void (*AudioPCMCallback)(double pts, uint8_t *stream, int len, void *context);

class IAudioPlay : public IObserver {
public:
    IAudioPlay(PlayerState *playerState);

    virtual ~IAudioPlay();

    //缓冲满后会阻塞
    void update(XData data) override;

    virtual bool startPlay(XParameter out) = 0;

    //获取缓冲数据，如没有就阻塞
    XData getData();

public:
    int maxFrameBuffer = 100;
    int pts;

protected:
    std::list<XData> frames;
    std::mutex framesMutex;
    std::condition_variable notEmpty;
    std::condition_variable notFull;
    AudioPCMCallback callback;  // 音频回调
    void *callbackContext = nullptr;
    PlayerState *playerState;

public:
    void setCallback(AudioPCMCallback pcmCallback, void *context);

    void stop() override;

};


#endif //XPLAY_IAUDIOPLAY_H
