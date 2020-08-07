//
// Created by ky611 on 2020/6/20.
//

#include "IPlayerBuilder.h"
#include "../demux/FFDemux.h"
#include "../decode/VideoDecode.h"
#include "../resample/FFResample.h"
#include "../audio/SLAudioPlay.h"
#include "../video/GLVideoView.h"

MediaPlayer *IPlayerBuilder::buildPlayer() {

    MediaPlayer *iplayer = createPlayer(0);

    auto *demux = createDemux(iplayer->playerState);

    IDecode *videoDecode = createVideoDecode(iplayer->playerState);
    IDecode *audioDecode = createAudioDecode(iplayer->playerState);
    demux->setVideoDecode(videoDecode);
    demux->setAudioDecode(audioDecode);

    //音频链路
    IResample *resample = createResample(iplayer->playerState);
    IAudioPlay *audioPlay = createAudioPlay(iplayer->playerState);
    IVideoView *videoView = createVideoView();

    iplayer->demux = demux;
    iplayer->audioPlay = audioPlay;
    iplayer->audioDecode = audioDecode;
    iplayer->videoDecode = videoDecode;
    iplayer->resample = resample;
    iplayer->videoView = videoView;
    return iplayer;
}
