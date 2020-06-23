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


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    const char *url = env->GetStringUTFChars(_url, 0);
    IPlayerProxy::get()->init();
    IPlayerProxy::get()->open(url);
    if (window) {
        IPlayerProxy::get()->initView(window);
    }
    IPlayerProxy::get()->start();
    return 0;
}

void add() {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_text(JNIEnv *env, jobject thiz) {

    auto *queue = new PacketQueue();
    auto test1 = new QueueTest(queue, "thread 1");
    auto test2 = new QueueTest(queue, "thread 2");
    auto test3 = new QueueTest(queue, "thread 3");
    auto test4 = new QueueTest(queue, "thread 4");
    test1->start();
    test2->start();
    test3->start();
    test4->start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    LOGI("queue size is %d", queue->getPacketSize());

}



extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_initView(JNIEnv *env, jobject thiz, jobject holder) {
    window = ANativeWindow_fromSurface(env, holder);
    LOGD("ANativeWindow_fromSurface %d", window);
    if (window) {
        IPlayerProxy::get()->initView(window);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_setMatrix(JNIEnv *env, jobject thiz, jfloatArray v_pmatrix) {

}