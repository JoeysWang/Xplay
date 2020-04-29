#include <jni.h>
#include <string>
#include "IDemux.h"
#include "FFDemux.h"
#include "XLog.h"
#include "XThread.h"


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    FFDemux *de = new FFDemux();


    short a = 10;
    int b = 100;
    int short_length = sizeof a;
    int int_length = sizeof(b);
    int long_length = sizeof(long);
    int char_length = sizeof(char);
    int double_length = sizeof(double);
    int float_length = sizeof(float);

    LOGD("short=%d, int=%d, long=%d, char=%d\n double=%d, float=%d", short_length, int_length,
         long_length,
         char_length, double_length, float_length);

    const char *url = env->GetStringUTFChars(_url, 0);
    de->open(url);
    de->start();
    XSleep(300);
    de->stop();

    return 0;
}