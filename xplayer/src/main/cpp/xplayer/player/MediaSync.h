//
// Created by 王越 on 2020/8/6.
//

#ifndef FFPLAYER_MEDIASYNC_H
#define FFPLAYER_MEDIASYNC_H


#include <memory>
#include "../data/PlayerState.h"
#include "XLog.h"
#include "../decode/VideoDecode.h"
#include "../decode/AudioDecode.h"
#include "../video/IVideoView.h"
#include "../resampler/Resampler.h"
#include "../../xhandler/XHandler.h"
#include "../dataStructure/MediaClock.h"
#include <thread>

class MediaSync {
public:
    MediaSync(
            std::shared_ptr<PlayerState> const &state,
            std::shared_ptr<VideoDecode> const &videoDecode,
            std::shared_ptr<AudioDecode> const &audioDecode
    );

    virtual ~MediaSync();

    void start();

    void pause();

    void stop();

    double calculateDelay(double delay);

    long getCurrentPosition();

    double getMasterClock();

    static void audioCallBack(double pts, uint8_t *stream, int len, void *context);

    void setResampler(const std::shared_ptr<Resampler> &resampler);

    void setVideoView(const std::shared_ptr<IVideoView> &videoView);

protected:
    std::mutex mutex;
    std::shared_ptr<PlayerState> playerState;
    std::shared_ptr<VideoDecode> videoDecode;
    std::shared_ptr<AudioDecode> audioDecode;
    std::shared_ptr<Resampler> resampler;
    std::shared_ptr<IVideoView> videoView;
    std::unique_ptr<MediaClock> audioClock;                 // 音频时钟
    std::unique_ptr<MediaClock> videoClock;
//    std::unique_ptr<XHandler> playerHandler = nullptr;
    bool isExist = false;
private:
    void audioPlay();

    void videoPlay();

    double maxFrameDuration;

    double lastFramePts = 0.0;
    double lastAudioPts = 0.0;

    bool resumeAfterPause = false;          // 暂停后恢复的
};


#endif //FFPLAYER_MEDIASYNC_H
