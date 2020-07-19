//
// Created by 王越 on 2020/7/3.
//

#ifndef XPLAY_MEDIASYNC2_H
#define XPLAY_MEDIASYNC2_H


#include "../decode/IDecode.h"
#include "../video/IVideoView.h"
#include "../resample/IResample.h"
#include "../audio/IAudioPlay.h"
#include "../../xhandler/HandlerThread.h"

class MediaSync2 {
public:
    MediaSync2(PlayerState *playerState, IDecode *audioDecode, IDecode *videoDecode);

    virtual ~MediaSync2();

    void pause();

    void stop();

    double calculateDelay(double delay);

    long getCurrentPosition();

    double getMasterClock();

    static void audioCallBack(double pts, uint8_t *stream, int len, void *context);

public:
    bool isExist = false;
public:
    PlayerState *playerState;
    IDecode *audioDecode;
    IDecode *videoDecode;
private:
    MediaClock *audioClock;                 // 音频时钟
    MediaClock *videoClock;                 // 视频时钟

    IResample *resample;
    IVideoView *videoView;
    IAudioPlay *iAudioPlay;
    double frameTimer;                      // 视频时钟
    XHandler *playerHandler = nullptr;

    double lastFramePts = 0.0;
    double lastAudioPts = 0.0;

    bool resumeAfterPause = false;          // 暂停后恢复的

public:
    void start();

    void setVideoView(IVideoView *videoView);

    void setResample(IResample *resample);

    void setAudioPlay(IAudioPlay *audioPlay);

    void setPlayerHandler(XHandler *playerHandler);

private:
    void audioPlay();

    void videoPlay();

    double maxFrameDuration;

};


#endif //XPLAY_MEDIASYNC2_H
