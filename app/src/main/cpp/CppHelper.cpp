//
// Created by 王越 on 2020/7/12.
//

#include <jni.h>
#include <thread>
#include "libyuv/scale_argb.h"
#include "xhandler/XLooper.h"
#include "xhandler/XHandler.h"
#include "XLog.h"
#include "xhandler/LooperManager.h"
#include "xhandler/HandlerThread.h"

class TH : public HandlerThread {
    void handleMessage(XMessage *message) override {
        LOGI("handleMessage  what=%d", message->what);
    }
};

TH *th;
static int count = 0;

void thfun() {
    auto *message = new XMessage();
    message->what = count++;
    th->getHandler()->sendMessage(message);

    std::chrono::milliseconds duration(15);
    std::this_thread::sleep_for(duration);
    auto *message2 = new XMessage();
    message2->what = count++;
    th->getHandler()->sendMessage(message2);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_CPPHelper_cpptest(JNIEnv *env, jobject thiz) {
//    if (!th) {
//        th = new TH();
//        std::thread thread(&thfun);
//        thread.detach();
//    }else
//        thfun();

}

