#include <jni.h>
#include <string>
#include "XLog.h"
#include "android/native_window_jni.h"
#include "../xhandler/XHandler.h"
#include "../metadata/Metadata.h"
#include "FFPlayer.h"
#include <thread>
#include <chrono>
#include <stdio.h>
#include <android/log.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavcodec/jni.h"
}


static JavaVM *javaVM = NULL;
struct fields_t {
    jfieldID context;
    jmethodID post_event;
};
static fields_t fields;
ANativeWindow *window;

static JNIEnv *getJNIEnv() {
    JNIEnv *env;
    assert(javaVM != NULL);
    if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return NULL;
    }
    return env;
}

static FFPlayer *getPlayerHander(JNIEnv *env, jobject thiz) {
    FFPlayer *const mp = (FFPlayer *) env->GetLongField(thiz, fields.context);
    return mp;
}

static FFPlayer *cachePlayerHander(JNIEnv *env, jobject thiz, long mediaPlayer) {
    FFPlayer *old = (FFPlayer *) env->GetLongField(thiz, fields.context);
    env->SetLongField(thiz, fields.context, mediaPlayer);
    return old;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer_nativeInit(JNIEnv *env, jclass clazz) {
    jclass jclass = env->FindClass("com/joeys/xplayer/XPlayer");
    if (jclass == NULL) { return; }
    fields.context = env->GetFieldID(jclass, "mNativeContext", "J");
    if (fields.context == NULL) { return; }
    fields.post_event = env->GetStaticMethodID(jclass, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        LOGE("postEventFromNative 方法在java层不存在");
        return;
    }
    env->DeleteLocalRef(jclass);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer_initPlayer(
        JNIEnv *env,
        jobject thiz,
        jobject weak_reference) {
    LOGI("XPlayer_initPlayer");
    auto player = new FFPlayer();
    cachePlayerHander(env, thiz, (long) player);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer_setDataSourceInternal(JNIEnv *env, jobject thiz, jstring _url) {
    auto player = getPlayerHander(env, thiz);
    const char *url = env->GetStringUTFChars(_url, 0);
    player->setDataSource(url);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer_initView(JNIEnv *env, jobject thiz, jobject holder) {
    auto player = getPlayerHander(env, thiz);
    window = ANativeWindow_fromSurface(env, holder);
    player->setNativeWindow(window);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer__1start(JNIEnv *env, jobject thiz) {
    auto player = getPlayerHander(env, thiz);
    if (player) { player->playOrPause(); }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer__1resume(JNIEnv *env, jobject thiz) {
    auto player = getPlayerHander(env, thiz);
    if (player) { player->playOrPause(); }
}


extern "C"
JNIEXPORT jint JNICALL
Java_com_joeys_xplayer_XPlayer__1getVideoWidth(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_joeys_xplayer_XPlayer__1getVideoHeight(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer__1stop(JNIEnv *env, jobject thiz) {

}
extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer__1release(JNIEnv *env, jobject thiz) {
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    getPlayerHander(env, thiz)->release();
    cachePlayerHander(env,thiz,0);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer__1pause(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    av_jni_set_java_vm(vm, NULL);
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}