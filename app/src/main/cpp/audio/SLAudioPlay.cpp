//
// Created by ky611 on 2020/6/16.
//

#include "SLAudioPlay.h"
#include "../XLog.h"
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES.h>

static SLObjectItf engineSL = NULL;
static SLEngineItf engineItf = NULL;
static SLObjectItf mix = NULL;
static SLObjectItf player = NULL;
static SLPlayItf iplayer = NULL;
static SLAndroidSimpleBufferQueueItf pcmQueue = NULL;

SLAudioPlay::SLAudioPlay() {
    buffer = new unsigned char[1024 * 1024];
}

SLEngineItf SLAudioPlay::createSL() {
    SLresult re;
    re = slCreateEngine(
            &engineSL, 0, 0, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        return NULL;
    }
    re = (*engineSL)->Realize(engineSL, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        return NULL;
    }
    re = (*engineSL)->GetInterface(engineSL, SL_IID_ENGINE, &engineItf);
    if (re != SL_RESULT_SUCCESS) {
        return NULL;
    }
    return engineItf;
}

static void pcmCallback(SLAndroidSimpleBufferQueueItf bf, void *context) {
    auto *ap = (SLAudioPlay *) context;
    if (!ap) {
        LOGE("pcmCallback SLAudioPlay is null");
        return;
    }
    LOGD("pcmCallback " );
    ap->playCall((void *) bf);
}

void SLAudioPlay::playCall(void *bufferQueue) {
//    if (!bufferQueue) { return; }
    auto bf = (SLAndroidSimpleBufferQueueItf) bufferQueue;

    XData data = getData();
    if (data.size <= 0) {
        LOGE("SLAudioPlay::playCall getData size =0 ");
        return;
    }

    LOGD("SLAudioPlay::playCall getData size = %d ",data.size);

    memcpy(buffer, data.data, data.size);

    (*bf)->Enqueue(bf, buffer, data.size);
    data.drop();
}

bool SLAudioPlay::startPlay(XParameter out) {
    engineItf = createSL();
    if (!engineItf) {
        LOGE("createSL ERROR");
        return false;
    }
    SLresult re = 0;
    re = (*engineItf)->CreateOutputMix(engineItf, &mix, 0, 0, 0);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix ERROR");
        return false;
    }
    re = (*mix)->Realize(mix, SL_BOOLEAN_FALSE);
    if (re != SL_RESULT_SUCCESS) {
        LOGE(" (*mix)->Realize ERROR");
        return false;
    }

    SLDataLocator_OutputMix outMix = {SL_DATALOCATOR_OUTPUTMIX, mix};
    SLDataSink audioSink = {&outMix, 0};

    //配置音频信息
    //缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 10};
    //音频格式
    LOGD("音频格式 channels=%d sampleRate=%d",out.channels,out.sampleRate);
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) (out.channels),
//            SL_SAMPLINGRATE_44_1,
            (SLuint32) (out.sampleRate * 1000),
            SL_PCMSAMPLEFORMAT_FIXED_32,
            SL_PCMSAMPLEFORMAT_FIXED_32,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,

            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource dataSource = {&queue, &pcm};

    //创建播放器

    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    re = (*engineItf)->CreateAudioPlayer(engineItf,
                                         &player,
                                         &dataSource,
                                         &audioSink,
                                         sizeof(ids) / sizeof(SLInterfaceID),
                                         ids,
                                         req);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("(*engineItf)->CreateAudioPlayer ERROR");
        return false;
    } else
        LOGD("(*engineItf)->CreateAudioPlayer success");

    (*player)->Realize(player, SL_BOOLEAN_FALSE);

    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("(*player)->GetInterface ERROR");
        return false;
    }
    LOGD("(*player)->GetInterface success");
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQueue);

    (*pcmQueue)->RegisterCallback(pcmQueue, pcmCallback, this);
    LOGD("(*player)->RegisterCallback success");

    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    LOGD("(*player)->SetPlayState success");
    (*pcmQueue)->Enqueue(pcmQueue, "", 1);

    LOGI("start audio play success");
    return true;

}

SLAudioPlay::~SLAudioPlay() {
    delete buffer;
}


