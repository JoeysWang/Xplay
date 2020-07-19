//
// Created by 王越 on 2020/4/26.
//

#include <thread>
#include "IDemux.h"
#include "../../XLog.h"

IDemux::IDemux(PlayerState *playerState) : playerState(playerState) {
    LOGI("IDemux constructor  ");
}

void IDemux::run() {
    readPacket();
}

void IDemux::setAudioDecode(IDecode *audioDecode) {
    IDemux::audioDecode = audioDecode;
}

void IDemux::setVideoDecode(IDecode *videoDecode) {
    IDemux::videoDecode = videoDecode;
}

IDemux::~IDemux() {
    mutex.lock();
    if (formatContext) {
        LOGD("~IDemux");
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
        formatContext = nullptr;
    }
    audioDecode = nullptr;
    videoDecode = nullptr;
    playerHandler = nullptr;
    mutex.unlock();

}

