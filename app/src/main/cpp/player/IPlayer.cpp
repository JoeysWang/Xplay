//
// Created by ky611 on 2020/6/20.
//

#include "IPlayer.h"
#include "../demux/IDemux.h"
#include "../decode/IDecode.h"
#include "../resample/IResample.h"
#include "../audio/IAudioPlay.h"
#include "../video/IVideoView.h"
#include "../XLog.h"

IPlayer *IPlayer::get(unsigned char index) {
    static IPlayer players[256];
    return &players[index];
}

void IPlayer::Main() {
    while (!isExit) {
        mutex.lock();
        //音频视频同步
        //获取音频pts，告诉视频
        if (!audioPlay || !videoDecode) {
            mutex.unlock();
            XSleep(2);
            continue;
        }
        mutex.unlock();
        XSleep(2);
    }
}

bool IPlayer::open(const char *path) {
    mutex.lock();
    if (!demux || !demux->open(path)) {
        LOGE("IPlayer::open demux error ");
        return false;
    }
    if (!videoDecode ||
        !videoDecode->openDecode(demux->getVideoParameter(),
                                 demux->getVideoStream(),
                                 demux->formatContext)) {
        LOGE("IPlayer videoDecode->open error ");
    }

    if (!audioDecode ||
        !audioDecode->openDecode(demux->getAudioParameter(),
                                 demux->getAudioStream(),
                                 demux->formatContext)) {
        LOGE("IPlayer audioDecode->open error ");
    }
    audioOutParam = (demux->getAudioParameter());

    if (!resample || !resample->open(demux->getAudioParameter(), audioOutParam)) {
        LOGE("IPlayer  resample->open error ");
    }
    if (window) {
        videoView->setRender(window);
    }
    mutex.unlock();
    LOGI("IPlayer::open success!");
    return true;
}

void IPlayer::start() {
    mutex.lock();
    if (!demux) {
        LOGE("IPlayer startPlay error ");
        mutex.unlock();
        return;
    }
    audioDecode->start();
    if (audioOutParam.sampleRate <= 0) {
        audioOutParam = demux->getAudioParameter();
    }
    audioPlay->startPlay(audioOutParam);

    playerState = new PlayerState();
    mediaSync = new MediaSync2(playerState, audioDecode, videoDecode);
    mediaSync->setAudioPlay(audioPlay);
    mediaSync->setResample(resample);

    videoDecode->start();
    resample->start();
    demux->start();

    mediaSync->setVideoView(videoView);

    mediaSync->start();
    resample->addObserver(audioPlay);

    XThread::start();
    mutex.unlock();
}


bool IPlayer::initView(void *window) {
    this->window = window;
    if (videoView) {
        videoView->setRender(window);
    }
    return true;
}

IPlayer::IPlayer() {

}

