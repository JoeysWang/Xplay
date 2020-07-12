//
// Created by 王越 on 2020/7/12.
//

#include <jni.h>
#include <thread>
#include "xhandler/XLooper.h"
#include "xhandler/XHandler.h"
#include "XLog.h"
#include "xhandler/LooperManager.h"


void getLooper(long id) {

    XLooper::prepare();
    auto handler = new XHandler();
    XLooper::loop();


}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_CPPHelper_cpptest(JNIEnv *env, jobject thiz) {
    std::thread t1(getLooper, 1);
    t1.detach();
    std::thread t2(getLooper, 2);
    t2.detach();
    std::thread t3(getLooper, 3);
    t3.detach();


}

