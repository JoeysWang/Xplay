#include <jni.h>
#include <string>
#include "demux/IDemux.h"
#include "demux/FFDemux.h"
#include "XLog.h"
#include "XThread.h"
#include "decode/FFDecode.h"
#include "android/native_window_jni.h"
#include "texture/XEGL.h"
#include "texture/XShader.h"
#include "video/IVideoView.h"
#include "video/GLVideoView.h"
#include "resample/IResample.h"
#include "resample/FFResample.h"

extern "C" {
#include "libavcodec/avcodec.h"
}


IVideoView *view = nullptr;
ANativeWindow *window;
jfloat *vpMatrix = nullptr;
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    auto *demux = new FFDemux();
    const char *url = env->GetStringUTFChars(_url, 0);
    demux->open(url);

    IDecode *videoDecode = new FFDecode();
    videoDecode->open(demux->getVideoParameter());
    videoDecode->addObserver(view);

    IDecode *audioDecode = new FFDecode();
    audioDecode->open(demux->getAudioParameter());

    IResample *resample = new FFResample();
    resample->open(demux->getAudioParameter());

    audioDecode->addObserver(resample);

    demux->addObserver(videoDecode);
    demux->addObserver(audioDecode);

    videoDecode->start();
    audioDecode->start();
    demux->start();

    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_initView(JNIEnv *env, jobject thiz, jobject holder) {
    window = ANativeWindow_fromSurface(env, holder);
    LOGD("ANativeWindow_fromSurface %d", window);
    if (window) {
        view = new GLVideoView();
        view->setRender(window);
        if (vpMatrix != nullptr)
            view->setMatrix(vpMatrix, sizeof(vpMatrix));
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_setMatrix(JNIEnv *env, jobject thiz, jfloatArray v_pmatrix) {
    if (view != nullptr) {
        vpMatrix = env->GetFloatArrayElements(v_pmatrix, 0);
        view->setMatrix(vpMatrix, 16);
    }
}