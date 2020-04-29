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

class Test : public IObserver {
public:
    void update(XData *data) override {
        AVPacket *avpac = data->data;
    }

};

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_joeys_xplay_Xplay_open(JNIEnv *env, jobject thiz, jstring _url) {
    using namespace std;
    FFDemux *de = new FFDemux();

    Test *pTest = new Test();
    de->addObserver(pTest);

    const char *url = env->GetStringUTFChars(_url, 0);
    de->open(url);
    de->start();

    IDecode *vdecode = new FFDecode();
    vdecode->open(de->getVideoParamter());

    XSleep(300);
    de->stop();

    return 0;
}