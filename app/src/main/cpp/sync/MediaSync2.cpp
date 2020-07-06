//
// Created by 王越 on 2020/7/3.
//

#include "MediaSync2.h"
#include <thread>

MediaSync2::MediaSync2(PlayerState *playerState, IDecode *audioDecode, IDecode *videoDecode)
        : playerState(playerState), audioDecode(audioDecode),
          videoDecode(videoDecode) {
    LOGI("MediaSync2 constructor playerState=%p", playerState);
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
        if (isExist || playerState->abortRequest) { break; }
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
//        LOGI("audio play");
        resample->update(*frameWrapper);
//        audioClock->setClock(frameWrapper->pts);
        audioDecode->getFrameQueue()->popFrame();
    }

}

void MediaSync2::videoPlay() {
    for (;;) {
        if (isExist || playerState->abortRequest) { break; }
        if (playerState->pauseRequest) {
            LOGI("MediaSync2::videoPlay sleep for pause");
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
//    LOGI("diff =%f sync_threshold=%f", diff, sync_threshold);
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

void MediaSync2::audioCallBack(double pts, uint8_t *stream, int len, void *context) {
    auto mediaSync2 = (MediaSync2 *) context;
    mediaSync2->audioClock->setClock(pts);
}
