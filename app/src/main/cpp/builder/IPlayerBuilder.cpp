//
// Created by ky611 on 2020/6/20.
//

#include "IPlayerBuilder.h"
#include "../demux/FFDemux.h"
#include "../decode/VideoDecode.h"
#include "../resample/FFResample.h"
#include "../audio/SLAudioPlay.h"
#include "../video/GLVideoView.h"

IPlayer *IPlayerBuilder::buildPlayer() {

    IPlayer *iplayer = createPlayer(0);

    auto *demux = createDemux(iplayer->playerState);

    IDecode *videoDecode = createVideoDecode(iplayer->playerState);
    IDecode *audioDecode = createAudioDecode(iplayer->playerState);
    demux->setVideoDecode(videoDecode);
    demux->setAudioDecode(audioDecode);

    //音频链路
    IResample *resample = createResample(iplayer->playerState);
    IAudioPlay *audioPlay = createAudioPlay(iplayer->playerState);
    //mediaSync自己去取数据帧
//    resample->addObserver(audioPlay);

    //视频链路
    IVideoView *videoView = createVideoView();
    //mediaSync自己去取数据帧
//    videoDecode->addObserver(videoView);


    iplayer->demux = demux;
    iplayer->audioPlay = audioPlay;
    iplayer->audioDecode = audioDecode;
    iplayer->videoDecode = videoDecode;
    iplayer->resample = resample;
    iplayer->videoView = videoView;
    return iplayer;
}
