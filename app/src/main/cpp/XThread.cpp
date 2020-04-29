//
// Created by 王越 on 2020/4/26.
//

#include "XThread.h"
#include "XLog.h"
#include "FFDemux.h"
#include <thread>
#include <chrono>

using namespace std;

void XSleep(int mis) {
    std::chrono::milliseconds duration(mis);
    std::this_thread::sleep_for(duration);
}

void XThread::start() {
    isExit = false;
    thread th(&XThread::ThreadMain, this);
    th.detach();
    string str = "aaa";

}



void XThread::stop() {
    isExit = true;
    for (int i = 0; i < 200; i++) {
        if (!isRunning) {
            LOGI("stop 线程 成功");
            return;
        }
        XSleep(10);
    }
    LOGE("stop 线程 超时");

}

void XThread::ThreadMain() {
    LOGD("线程函数进入");
    isRunning = true;
    Main();
    isRunning = false;
    LOGD("线程函数退出");


}
