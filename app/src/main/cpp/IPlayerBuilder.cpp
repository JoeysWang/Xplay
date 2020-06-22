//
// Created by ky611 on 2020/6/20.
//

#include "IPlayerBuilder.h"
#include "demux/FFDemux.h"
#include "decode/FFDecode.h"
#include "resample/FFResample.h"
#include "audio/SLAudioPlay.h"
#include "video/GLVideoView.h"

IPlayer *IPlayerBuilder::buildPlayer() {

    IPlayer *iplayer = createPlayer(0);

    auto *demux = createDemux();

    IDecode *videoDecode = createDecode();
    IDecode *audioDecode = createDecode();
    demux->addObserver(videoDecode);
    demux->addObserver(audioDecode);

    //音频链路
    IResample *resample = createResample();
    audioDecode->addObserver(resample);
    IAudioPlay *audioPlay = createAudioPlay();
    resample->addObserver(audioPlay);

    //视频链路
    IVideoView *videoView = new GLVideoView();
    videoDecode->addObserver(videoView);

    iplayer->demux = demux;
    iplayer->audioPlay = audioPlay;
    iplayer->audioDecode = audioDecode;
    iplayer->videoDecode = videoDecode;
    iplayer->resample = resample;
    iplayer->videoView = videoView;
    return iplayer;

}
