#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"
#include "XThread.h"


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    std::string hello = "Hello from C++";

    FFDemux *de = new FFDemux();

    const char *url = env->GetStringUTFChars(_url, 0);
    de->open(url);
    de->start();
    XSleep(300);
    de->stop();

    return 0;
}