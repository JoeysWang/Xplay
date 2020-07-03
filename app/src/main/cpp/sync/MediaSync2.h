//
// Created by 王越 on 2020/7/3.
//

#ifndef XPLAY_MEDIASYNC2_H
#define XPLAY_MEDIASYNC2_H


#include "../decode/IDecode.h"
#include "../resample/IResample.h"
#include "../video/IVideoView.h"

class MediaSync2 {
public:
    MediaSync2(PlayerState *playerState, IDecode *audioDecode, IDecode *videoDecode);

    double calculateDelay(double delay);

    double getMasterClock();

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

    double lastFramePts = 0.0;

public:
    void start();

    void setVideoView(IVideoView *videoView);

    void setResample(IResample *resample);

private:
    void audioPlay();

    void videoPlay();

    double maxFrameDuration;
    double frameTimer;
};


#endif //XPLAY_MEDIASYNC2_H
