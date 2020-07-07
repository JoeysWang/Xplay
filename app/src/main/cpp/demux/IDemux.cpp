//
// Created by 王越 on 2020/4/26.
//

#include <thread>
#include "IDemux.h"
#include "../XLog.h"

IDemux::IDemux(PlayerState *playerState) : playerState(playerState) {
    LOGI("IDemux constructor %p", playerState);
}

void IDemux::run() {
    while (!isExit || !playerState->abortRequest) {
        if (playerState->pauseRequest) {
            LOGI("IDemux sleep for pause");
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);
            continue;
        }
        XData d = read();
        if (d.size > 0)
            notify(d);
    }
}

IDemux::~IDemux() {
    if (formatContext) {
        LOGD("~IDemux");
        avformat_close_input(&formatContext);
        avformat_free_context(formatContext);
        formatContext = nullptr;
    }


}

