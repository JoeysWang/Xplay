#include <jni.h>
#include <string>
#include "../../XLog.h"
#include "android/native_window_jni.h"
#include "player/MediaPlayer.h"
#include "builder/IPlayerBuilder.h"
#include "builder/FFPlayerBuilder.h"
#include "test/QueueTest.h"
#include "../xhandler/XHandler.h"
#include "../metadata/Metadata.h"
#include <thread>
#include <chrono>

extern "C" {
#include "libavcodec/avcodec.h"
#include "../include/armeabi-v7a/libavcodec/jni.h"

}
struct fields_t {
    jfieldID context;
    jmethodID post_event;
};
static fields_t fields;
ANativeWindow *window;
static JavaVM *javaVM = NULL;

static JNIEnv *getJNIEnv() {
    JNIEnv *env;
    assert(javaVM != NULL);
    if (javaVM->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return NULL;
    }
    return env;
}

struct retriever_fields_t {
    jfieldID context;
};

class JNIMediaPlayerListener : public MediaPlayerListener {
public:
    JNIMediaPlayerListener(JNIEnv *env, jobject thiz, jobject weak_thiz);

    ~JNIMediaPlayerListener();

    void notify(int msg, int ext1, int ext2, void *obj) override;

private:
    JNIMediaPlayerListener();

    jclass mClass;
    jobject mObject;
};


static MediaPlayer *getMediaPlayer(JNIEnv *env, jobject thiz) {
    MediaPlayer *const mp = (MediaPlayer *) env->GetLongField(thiz, fields.context);
    return mp;
}

static MediaPlayer *setMediaPlayer(JNIEnv *env, jobject thiz, long mediaPlayer) {
    MediaPlayer *old = (MediaPlayer *) env->GetLongField(thiz, fields.context);
    env->SetLongField(thiz, fields.context, mediaPlayer);
    return old;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_00024Companion_native_1init(JNIEnv *env, jobject thiz) {
    jclass clazz = env->FindClass("com/joeys/xplay/Xplay");
    if (clazz == NULL) {
        return;
    }
    fields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (fields.context == NULL) {
        return;
    }
    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        return;
    }

    env->DeleteLocalRef(clazz);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_iplayerIinit(
        JNIEnv *env,
        jobject thiz,
        jobject weakReference) {
    LOGI("Xplay_iplayerIinit");
    MediaPlayer *pPlayer = FFPlayerBuilder::get()->buildPlayer();
    JNIMediaPlayerListener *listener = new JNIMediaPlayerListener(env, thiz, weakReference);
    pPlayer->setListener(listener);

    setMediaPlayer(env, thiz, (long) pPlayer);
}

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

    env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
                              msg, ext1, ext2, obj);

    if (env->ExceptionCheck()) {
        LOGE("An exception occurred while notifying an event.");
        env->ExceptionClear();
    }

    if (status) {
        javaVM->DetachCurrentThread();
    }
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay__1setDataSource(JNIEnv *env, jobject thiz, jstring _url) {

    const char *url = env->GetStringUTFChars(_url, 0);
    auto s = new std::string(url);
    getMediaPlayer(env, thiz)->setDataSource(*s);
    if (window) {
        getMediaPlayer(env, thiz)->initView(window);
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay__1start(JNIEnv *env, jobject thiz) {
    getMediaPlayer(env, thiz)->openSource();
    getMediaPlayer(env, thiz)->start();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_text(JNIEnv *env, jobject thiz) {
}


extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_initView(JNIEnv *env, jobject thiz, jobject holder) {
    window = ANativeWindow_fromSurface(env, holder);
    LOGD("ANativeWindow_fromSurface %p", window);
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
    ANativeWindow_release(window);
    window = nullptr;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_joeys_xplay_Xplay__1getVideoWidth(JNIEnv *env, jobject thiz) {
    return getMediaPlayer(env, thiz)->getVideoWidth();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_joeys_xplay_Xplay__1getVideoHeight(JNIEnv *env, jobject thiz) {
    return getMediaPlayer(env, thiz)->getVideoHeight();
}

extern "C"
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    av_jni_set_java_vm(vm, NULL);
    javaVM = vm;
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
//    if (register_com_cgfay_media_CainMediaPlayer(env) != JNI_OK) {
//        return -1;
//    }
    return JNI_VERSION_1_4;
}