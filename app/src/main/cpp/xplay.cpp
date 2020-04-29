#include <jni.h>
#include <string>
#include "demux/IDemux.h"
#include "demux/FFDemux.h"
#include "XLog.h"
#include "XThread.h"
#include "decode/FFDecode.h"

extern "C" {
#include "libavcodec/avcodec.h"
}



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

    demux->start();

    videoDecode->start();
    audioDecode->start();

    demux->addObserver(videoDecode);
    demux->addObserver(audioDecode);

    return 0;
}