//
// Created by 王越 on 2020/7/3.
//

#include "MediaSync2.h"
#include "../player/PlayerMessage.h"
#include <thread>

MediaSync2::MediaSync2(PlayerState *playerState, IDecode *audioDecode, IDecode *videoDecode)
        : playerState(playerState), audioDecode(audioDecode),
          videoDecode(videoDecode) {
    audioClock = new MediaClock;
    videoClock = new MediaClock;
    maxFrameDuration = 10.0;
}

void MediaSync2::start() {
    isExist = false;
    std::thread audioThread(&MediaSync2::audioPlay, this);
    std::thread videoThread(&MediaSync2::videoPlay, this);
    audioThread.detach();
    videoThread.detach();
}

void MediaSync2::audioPlay() {
    for (;;) {
        if (isExist || playerState->abortRequest) { return; }
        if (playerState->pauseRequest) {
            LOGI("MediaSync2::audioPlay sleep for pause");
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        XData *frameWrapper = audioDecode->getFrameQueue()->currentFrame();
        if (frameWrapper->size == 0 || lastFramePts == 0) {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);
            continue;
        }
        resample->update(*frameWrapper);
        audioDecode->getFrameQueue()->popFrame();
    }
}

void MediaSync2::audioCallBack(double pts, uint8_t *stream, int len, void *context) {
    auto mediaSync2 = (MediaSync2 *) context;
    mediaSync2->lastAudioPts = pts;
    mediaSync2->audioClock->setClock(pts);
    if (mediaSync2->playerHandler) {
        mediaSync2->playerHandler->postMessage(MSG_CURRENT_POSITON,
                                               mediaSync2->getCurrentPosition(),
                                               0);
    }
}


long MediaSync2::getCurrentPosition() {
    // 起始延时
    int64_t start_time = videoDecode->formatCtx->start_time;
    int64_t start_diff = 0;
    if (start_time > 0 && start_time != AV_NOPTS_VALUE) {
        start_diff = av_rescale(start_time, 1000, AV_TIME_BASE);
    }

    // 计算主时钟的时间
    int64_t pos = 0;
    double clock = getMasterClock();
    if (isnan(clock)) {
        pos = playerState->seekPos;
    } else {
        pos = (int64_t) (clock * 1000);
    }
    if (pos < 0 || pos < start_diff) {
        return 0;
    }
    return (long) (pos - start_diff);
}

void MediaSync2::videoPlay() {
    for (;;) {
        if (isExist || playerState->abortRequest) { return; }
        if (playerState->pauseRequest) {
//            LOGI("MediaSync2::videoPlay sleep for pause");
            resumeAfterPause = true;
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }

        XData *frameWrapper = videoDecode->getFrameQueue()->currentFrame();
        if (!frameWrapper || frameWrapper->size == 0) {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);
            continue;
        }

        if (resumeAfterPause) {
            resumeAfterPause = false;
            videoClock->setClock(lastFramePts);
            audioClock->setClock(lastAudioPts);
        }

        double current_pts = frameWrapper->pts;
        double lastDuration = current_pts - lastFramePts;
        double delay = calculateDelay(lastDuration);

        if (delay > 0) {
            std::chrono::milliseconds duration((int) (delay * 1000));
            std::this_thread::sleep_for(duration);
        }
        videoClock->setClock(frameWrapper->pts);
        lastFramePts = frameWrapper->pts;
        videoView->render(frameWrapper);
        videoDecode->getFrameQueue()->popFrame();
    }
}

double MediaSync2::calculateDelay(double delay) {
    double sync_threshold, diff = 0;
    diff = videoClock->getClock() - getMasterClock();
    sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
    if (!isnan(diff) && fabs(diff) < maxFrameDuration) {
        if (diff <= -sync_threshold) {
            delay = FFMAX(0, delay + diff);
        } else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD) {
            delay = delay + diff;
        } else if (diff >= sync_threshold) {
            delay = 2 * delay;
        }
    }
    return delay;
}


double MediaSync2::getMasterClock() {
    return audioClock->getClock();
}

void MediaSync2::setResample(IResample *pResample) {
    MediaSync2::resample = pResample;
}

void MediaSync2::setVideoView(IVideoView *videoView) {
    MediaSync2::videoView = videoView;
}

void MediaSync2::setAudioPlay(IAudioPlay *pAudioPlay) {
    MediaSync2::iAudioPlay = pAudioPlay;
    iAudioPlay->setCallback(MediaSync2::audioCallBack, (void *) this);
}

void MediaSync2::stop() {
    isExist = true;
    playerHandler = nullptr;
}

MediaSync2::~MediaSync2() {
    delete audioClock;
    delete videoClock;
}

void MediaSync2::setPlayerHandler(XHandler *playerHandler) {
    MediaSync2::playerHandler = playerHandler;
}

