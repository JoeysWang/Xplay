#include <jni.h>
#include <string>
#include "XLog.h"
#include "android/native_window_jni.h"
#include "../xhandler/XHandler.h"
#include "../metadata/Metadata.h"
#include "FFPlayer.h"
#include "utils/MediaPlayerListener.h"
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
    jfieldID nativePointer;
    jmethodID postEventFunction;
};
static fields_t javaFields;
ANativeWindow *window;

static JNIEnv *getJNIEnv() {
    JNIEnv *env;
    assert(javaVM != NULL);
    if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return NULL;
    }
    return env;
}

class JNIMediaPlayerListener : public MediaPlayerListener {
public:
    JNIMediaPlayerListener(JNIEnv *env, jobject thiz, jobject weak_thiz);

    ~JNIMediaPlayerListener();

    void notify(int msg, int ext1, int ext2, void *obj) override;

private:
    JNIMediaPlayerListener() {};

    jclass mClass;
    jobject mObject;
};

JNIMediaPlayerListener::JNIMediaPlayerListener(JNIEnv *env, jobject thiz, jobject weak_thiz) {
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        LOGE("Can't find com.joeys.xplay.Xplay.kt");
        return;
    }
    mClass = (jclass) env->NewGlobalRef(clazz);
    mObject = env->NewGlobalRef(weak_thiz);
}

JNIMediaPlayerListener::~JNIMediaPlayerListener() {
    JNIEnv *env = getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2, void *obj) {
    JNIEnv *env = getJNIEnv();
    bool status = (javaVM->AttachCurrentThread(&env, NULL) >= 0);

    env->CallStaticVoidMethod(mClass, javaFields.postEventFunction, mObject,
                              msg, ext1, ext2, obj);

//    if (env->ExceptionCheck()) {
//        LOGE("An exception occurred while notifying an event.");
//        env->ExceptionClear();
//    }

    if (status) {
        javaVM->DetachCurrentThread();
    }
}


static FFPlayer *getPlayerHander(JNIEnv *env, jobject thiz) {
    FFPlayer *const mp = (FFPlayer *) env->GetLongField(thiz, javaFields.nativePointer);
    return mp;
}

static FFPlayer *cachePlayerHander(JNIEnv *env, jobject thiz, long mediaPlayer) {
    FFPlayer *old = (FFPlayer *) env->GetLongField(thiz, javaFields.nativePointer);
    env->SetLongField(thiz, javaFields.nativePointer, mediaPlayer);
    return old;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplayer_XPlayer_nativeInit(JNIEnv *env, jclass clazz) {
    jclass jclass = env->FindClass("com/joeys/xplayer/XPlayer");
    if (jclass == NULL) { return; }
    javaFields.nativePointer = env->GetFieldID(jclass, "mNativeContext", "J");
    if (javaFields.nativePointer == NULL) {
        return;
    }
    javaFields.postEventFunction = env->GetStaticMethodID(jclass, "postEventFromNative",
                                                          "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (javaFields.postEventFunction == NULL) {
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
    auto listener = std::make_shared<JNIMediaPlayerListener>(env, thiz, weak_reference);
    player->setJavaHandler(listener);
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
    cachePlayerHander(env, thiz, 0);
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