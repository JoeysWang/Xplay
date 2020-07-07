//
// Created by 王越 on 2020/7/3.
//

#ifndef XPLAY_MEDIASYNC2_H
#define XPLAY_MEDIASYNC2_H


#include "../decode/IDecode.h"
#include "../video/IVideoView.h"
#include "../resample/IResample.h"
#include "../audio/IAudioPlay.h"

class MediaSync2 {
public:
    MediaSync2(PlayerState *playerState, IDecode *audioDecode, IDecode *videoDecode);

    virtual ~MediaSync2();

    void stop();

    double calculateDelay(double delay);

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

    double lastFramePts = 0.0;

public:
    void start();

    void setVideoView(IVideoView *videoView);

    void setResample(IResample *resample);

    void setAudioPlay(IAudioPlay *audioPlay);

private:
    void audioPlay();

    void videoPlay();

    double maxFrameDuration;
    double frameTimer;
};


#endif //XPLAY_MEDIASYNC2_H
