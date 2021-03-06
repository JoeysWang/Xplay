//
// Created by 王越 on 2020/4/26.
//

#include "XThread.h"
#include "../../XLog.h"
#include "demux/FFDemux.h"
#include <thread>
#include <chrono>

using namespace std;

void XSleep(int mis) {
    std::chrono::milliseconds duration(mis);
    std::this_thread::sleep_for(duration);
}

void XThread::start() {
    isExit = false;
    thread th(&XThread::threadMain, this);
    th.detach();
}

void XThread::stop() {
    isExit = true;
}

void XThread::threadMain() {
    isRunning = true;
    run();
    isRunning = false;
    LOGD("线程函数退出");
}
