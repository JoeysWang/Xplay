#include <jni.h>
#include <string>
#include "XLog.h"
#include "android/native_window_jni.h"
#include "player/IPlayer.h"
#include "builder/IPlayerBuilder.h"
#include "builder/FFPlayerBuilder.h"
#include "player/IPlayerProxy.h"
#include "test/QueueTest.h"
#include <thread>
#include <chrono>

extern "C" {
#include "libavcodec/avcodec.h"
}

ANativeWindow *window;
jfieldID context;

static IPlayer *getMediaPlayer(JNIEnv *env, jobject thiz) {
    IPlayer *const mp = (IPlayer *) env->GetLongField(thiz, context);
    return mp;
}

static IPlayer *setMediaPlayer(JNIEnv *env, jobject thiz, long mediaPlayer) {
    IPlayer *old = (IPlayer *) env->GetLongField(thiz, context);
    env->SetLongField(thiz, context, mediaPlayer);
    return old;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_iplayerIinit(JNIEnv *env, jobject thiz) {
    LOGI("Xplay_iplayerIinit");
    jclass clazz = env->FindClass("com/joeys/xplay/Xplay");
    context = env->GetFieldID(clazz, "mNativeContext", "J");
    IPlayer *pPlayer = FFPlayerBuilder::get()->buildPlayer();
    setMediaPlayer(env, thiz, (long) pPlayer);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    const char *url = env->GetStringUTFChars(_url, 0);
    getMediaPlayer(env, thiz)->open(url);
    if (window) {
        getMediaPlayer(env, thiz)->initView(window);
    }
    getMediaPlayer(env, thiz)->start();
    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_text(JNIEnv *env, jobject thiz) {
    auto test1 = new QueueTest();
    test1->start();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_initView(JNIEnv *env, jobject thiz, jobject holder) {
    window = ANativeWindow_fromSurface(env, holder);
    LOGD("ANativeWindow_fromSurface %d", window);
    if (window) {
        getMediaPlayer(env, thiz)->initView(window);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_setMatrix(JNIEnv *env, jobject thiz, jfloatArray v_pmatrix) {

}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay__1pause(JNIEnv *env, jobject thiz) {
    getMediaPlayer(env, thiz)->pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay__1resume(JNIEnv *env, jobject thiz) {
    getMediaPlayer(env, thiz)->resume();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay__1stop(JNIEnv *env, jobject thiz) {
    getMediaPlayer(env, thiz)->stop();

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay__1release(JNIEnv *env, jobject thiz) {
    getMediaPlayer(env, thiz)->release();
}
