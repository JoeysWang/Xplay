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

extern "C" {
#include "libavcodec/avcodec.h"
}


IVideoView *view;
ANativeWindow *window;

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    FFDemux *demux = new FFDemux();
    const char *url = env->GetStringUTFChars(_url, 0);
    demux->open(url);

    IDecode *videoDecode = new FFDecode();
    videoDecode->open(demux->getVideoParameter());

    IDecode *audioDecode = new FFDecode();
    audioDecode->open(demux->getAudioParameter());

    videoDecode->addObserver(view);
    demux->start();

    videoDecode->start();
    audioDecode->start();

    demux->addObserver(videoDecode);
    demux->addObserver(audioDecode);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_joeys_xplay_Xplay_initView(JNIEnv *env, jobject thiz, jobject holder) {
    window = ANativeWindow_fromSurface(env, holder);
    LOGD("ANativeWindow_fromSurface %d",window);
    view = new GLVideoView();
    view->setRender(window);

}