//
// Created by 王越 on 2020/8/6.
//

#include "MediaSync.h"
#include "PlayerMessage.h"

MediaSync::MediaSync(std::shared_ptr<PlayerState> const &state,
                     std::shared_ptr<VideoDecode> const &videoDecode,
                     std::shared_ptr<AudioDecode> const &audioDecode) {
    playerState = state;
    this->videoDecode = videoDecode;
    this->audioDecode = audioDecode;

    audioClock = std::make_unique<MediaClock>();
    videoClock = std::make_unique<MediaClock>();
    maxFrameDuration = 10.0;

}

void MediaSync::start() {
    std::thread audioThread(&MediaSync::audioPlay, this);
    std::thread videoThread(&MediaSync::videoPlay, this);
    audioThread.detach();
    videoThread.detach();
}

void MediaSync::pause() {

}

void MediaSync::setResampler(const std::shared_ptr<Resampler> &resampler) {
    MediaSync::resampler = resampler;
    resampler->setCallback(audioCallBack, this);
}

void MediaSync::setVideoView(const std::shared_ptr<IVideoView> &videoView) {
    MediaSync::videoView = videoView;
}

void MediaSync::stop() {
    isExit = true;
    std::unique_lock<std::mutex> lockVideo(videoThreadMutex);
    while (videoPlaying) {
        videoPlayingWaitting.wait(lockVideo);
    }
    lockVideo.unlock();

    std::unique_lock<std::mutex> lockAudio(audioThreadMutex);
    while (audioPlaying) {
        audioPlayingWaitting.wait(lockAudio);
    }
    lockAudio.unlock();
    LOGI("MediaSync::stop success");
}

void MediaSync::audioPlay() {
    while (true) {
        audioPlaying = true;
        if (isExit || playerState->abortRequest) {
            LOGI("MediaSync::audioPlay isExit=true");
            resampler->quit();
            break;
        }
        if (playerState->pauseRequest) {
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        FrameData *frame = nullptr;
        audioDecode->popFrame(frame);
        if (!frame || frame->size == 0) {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);
            continue;
        }
        resampler->update(frame);
    }
    audioPlaying = false;
    audioPlayingWaitting.notify_all();
    LOGI("MediaSync::audioPlay 退出");
}

void MediaSync::videoPlay() {
    while (true) {
        videoPlaying = true;
        if (isExit || playerState->abortRequest) {
            LOGI("MediaSync::videoPlay isExit=true");
            videoView->quit();
            break;
        }
        if (playerState->pauseRequest) {
            resumeAfterPause = true;
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        FrameData *frameWrapper = nullptr;
        videoDecode->popFrame(frameWrapper);
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
    }
    videoPlaying = false;
    videoPlayingWaitting.notify_all();

    LOGI("MediaSync::videoPlay 退出");
}

double MediaSync::calculateDelay(double delay) {
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

long MediaSync::getCurrentPosition() {
    // 起始延时
    int64_t start_time = videoDecode->formatContext->start_time;
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

double MediaSync::getMasterClock() {
    return audioClock->getClock();
}

void MediaSync::audioCallBack(double pts, uint8_t *stream, int len, void *context) {
    auto mediaSync = (MediaSync *) context;
    mediaSync->lastAudioPts = pts;
    mediaSync->audioClock->setClock(pts);
//    if (mediaSync->playerHandler) {
//        mediaSync->playerHandler->postMessage(MSG_CURRENT_POSITON,
//                                              mediaSync->getCurrentPosition(),
//                                              0);
//    }
}

MediaSync::~MediaSync() {
    LOGI("MediaSync::~MediaSync");
}
