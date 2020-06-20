#include <jni.h>
#include <string>
#include "XLog.h"
#include "android/native_window_jni.h"
#include "player/IPlayer.h"
#include "IPlayerBuilder.h"
#include "FFPlayerBuilder.h"


extern "C" {
#include "libavcodec/avcodec.h"
}


ANativeWindow *window;

static IPlayer *player = NULL;


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    const char *url = env->GetStringUTFChars(_url, 0);

    player = FFPlayerBuilder::get()->buildPlayer();

    player->open(url);
    if (window) {
        player->initView(window);
    }

    player->start();

    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_initView(JNIEnv *env, jobject thiz, jobject holder) {
    window = ANativeWindow_fromSurface(env, holder);
    LOGD("ANativeWindow_fromSurface %d", window);
    if (window && player) {
        player->initView(window);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_setMatrix(JNIEnv *env, jobject thiz, jfloatArray v_pmatrix) {

}