//
// Created by ky611 on 2020/6/16.
//

#include "SLAudioPlay.h"
#include "../XLog.h"
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES.h>


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
    auto *slAudioPlayer = (SLAudioPlay *) context;
    if (!slAudioPlayer) {
        LOGE("pcmCallback SLAudioPlay is null");
        return;
    }
    slAudioPlayer->playCall((void *) bf);

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
    LOGD("音频格式 channels=%d sampleRate=%d", out.channels, out.sampleRate);
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            (SLuint32) out.channels,
            getCurSampleRate(out.sampleRate),
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志

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
    }
//    else
//        LOGD("(*engineItf)->CreateAudioPlayer success");

    (*player)->Realize(player, SL_BOOLEAN_FALSE);

    re = (*player)->GetInterface(player, SL_IID_PLAY, &iplayer);
    if (re != SL_RESULT_SUCCESS) {
        LOGE("(*player)->GetInterface ERROR");
        return false;
    }
    re = (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &pcmQueue);
    (*pcmQueue)->RegisterCallback(pcmQueue, pcmCallback, this);
    (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);
    (*pcmQueue)->Enqueue(pcmQueue, "", 1);

    return true;

}

void SLAudioPlay::playCall(void *bufferQueue) {
    auto bf = (SLAndroidSimpleBufferQueueItf) bufferQueue;
    XData data = getData();
    if (data.size <= 0) {
        LOGE("SLAudioPlay::playCall getData size =0 ");
        return;
    }

    memcpy(buffer, data.resampleData, data.size);

    (*bf)->Enqueue(bf, buffer, data.size);
    if (callback) {
        callback(data.pts, buffer, data.size, callbackContext);
    }
    data.drop();
}

SLuint32 SLAudioPlay::getCurSampleRate(int sample_rate) {
    SLuint32 rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    return rate;
}

SLAudioPlay::~SLAudioPlay() {
    mutex.lock();
    LOGI("~SLAudioPlay");
    delete buffer;
    engineSL = NULL;
    engineItf = NULL;
    mix = NULL;
    player = NULL;
    mutex.unlock();
}


