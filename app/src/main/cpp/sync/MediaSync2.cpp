//
// Created by 王越 on 2020/7/3.
//

#include "MediaSync2.h"
#include <thread>

MediaSync2::MediaSync2(PlayerState *playerState, IDecode *audioDecode, IDecode *videoDecode)
        : playerState(playerState), audioDecode(audioDecode), videoDecode(videoDecode) {
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
        if (isExist) { break; }
        XData *frameWrapper = audioDecode->getFrameQueue()->currentFrame();
        if (frameWrapper->size == 0) {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);
            continue;
        }
        resample->update(*frameWrapper);
        audioClock->setClock(frameWrapper->pts);
        audioDecode->getFrameQueue()->popFrame();
    }

}

void MediaSync2::videoPlay() {
    for (;;) {
        if (isExist) { break; }
        XData *frameWrapper = videoDecode->getFrameQueue()->currentFrame();
        if (frameWrapper->size == 0) {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);
            continue;
        }
        double current_pts = frameWrapper->pts;

        double lastDuration = current_pts - lastFramePts;
        double delay = calculateDelay(lastDuration);
        LOGI("video delay %f", delay);
        if (delay > 0) {
            std::chrono::milliseconds duration((int) (delay * 1000));
            std::this_thread::sleep_for(duration);
        }
//// 处理超过延时阈值的情况
//        if (fabs(delay) > AV_SYNC_THRESHOLD_MAX) {
//            if (delay > 0) {
//                delay = AV_SYNC_THRESHOLD_MAX;
//            } else {
//                delay = 0;
//            }
//        }
//        double time = av_gettime_relative() / 1000000.0;
//        if (isnan(frameTimer) || time < frameTimer) {
//            frameTimer = time;
//        }
//        // 如果当前时间小于帧计时器的时间 + 延时时间，则表示还没到当前帧
//        if (time < frameTimer + delay) {
//            *remaining_time = FFMIN(frameTimer + delay - time, *remaining_time);
//            break;
//        }
//        // 更新帧计时器
//        frameTimer += delay;
//        // 帧计时器落后当前时间超过了阈值，则用当前的时间作为帧计时器时间
//        if (delay > 0 && time - frameTimer > AV_SYNC_THRESHOLD_MAX) {
//            frameTimer = time;
//        }

        videoClock->setClock(frameWrapper->pts);
        lastFramePts = frameWrapper->pts;
        videoView->update(*frameWrapper);

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

void MediaSync2::setResample(IResample *resample) {
    MediaSync2::resample = resample;
}

void MediaSync2::setVideoView(IVideoView *videoView) {
    MediaSync2::videoView = videoView;
}
