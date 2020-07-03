//
// Created by cain on 2018/12/30.
//

#ifndef MEDIASYNC_H
#define MEDIASYNC_H


#include "../video/IVideoView.h"
#include "../decode/VideoDecode.h"
#include "MediaClock.h"
#include "../player/PlayerState.h"
#include "../audio/IAudioPlay.h"
#include "../resample/IResample.h"

/**
 * 视频同步器
 */
class MediaSync : public IObserver {

public:
    MediaSync(PlayerState *playerState);

    virtual ~MediaSync();

    void reset();

    void start(IDecode *videoDecoder, IDecode *audioDecoder);

    void stop();

    // 设置视频输出设备
    void setVideoDevice(IVideoView *device);

    // 设置帧最大间隔
    void setMaxDuration(double maxDuration);

    // 更新视频帧的计时器
    void refreshVideoTimer();

    // 更新音频时钟
    void updateAudioClock(double pts, double time);

    // 获取音频时钟与主时钟的差值
    double getAudioDiffClock();

    // 更新外部时钟
    void updateExternalClock(double pts);

    double getMasterClock();


    MediaClock *getAudioClock();

    MediaClock *getVideoClock();

    MediaClock *getExternalClock();

    void Main() override;

    void update(XData data) override;

    void setAudioDevice(IAudioPlay *audioDevice);

    void setResample(IResample *resample);

private:
    void refreshVideo(double *remaining_time);

    void checkExternalClockSpeed();

    double calculateDelay(double delay);

    double calculateDuration(XData *vp, XData *nextvp);

    void renderVideo();

private:
    PlayerState *playerState;               // 播放器状态
    bool abortRequest;                      // 停止
    bool mExit;

    double lastPlay;
    long playTimeST = 0L;
    MediaClock *audioClock;                 // 音频时钟
    MediaClock *videoClock;                 // 视频时钟
    MediaClock *extClock;                   // 外部时钟

    IDecode *videoDecoder;             // 视频解码器
    IDecode *audioDecoder;             // 视频解码器

    std::mutex mMutex;
    std::condition_variable mCondition;

    int forceRefresh;                       // 强制刷新标志
    double maxFrameDuration;                // 最大帧延时
    int frameTimerRefresh;                  // 刷新时钟
    double frameTimer;                      // 视频时钟

    IVideoView *videoDevice;               // 视频输出设备
    IAudioPlay *audioDevice;               // 音频输出设备
    IResample *resample;

    AVFrame *pFrameARGB;
    uint8_t *mBuffer;

};


#endif //MEDIASYNC_H
