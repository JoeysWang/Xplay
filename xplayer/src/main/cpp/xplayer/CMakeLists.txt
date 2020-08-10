
add_library(
        xplay

        SHARED

        player_android.cpp

        FFPlayer.cpp
        demux/Demuxer.cpp

        decode/AudioDecode.cpp
        decode/VideoDecode.cpp
        decode/IDecode.cpp

        player/MediaSync.cpp

        data/PlayerState.cpp
        data/DecodeParam.cpp
        data/PacketData.cpp
        data/FrameData.cpp

        dataStructure/Queue.cpp
        dataStructure/FrameQueue.cpp
        dataStructure/MediaClock.cpp

        resampler/Resampler.cpp
        video/IVideoView.cpp
        video/GLVideoView.cpp
        video/texture/XEGL.cpp
        video/texture/XTexture.cpp
        video/texture/XShader.cpp
        audio/SLAudioPlayer.cpp
        audio/IAudioPlay.cpp

        utils/CoordinateUtils.cpp
        utils/OpenGLUtils.cpp
        utils/vecmath.cpp
        utils/MediaPlayerListener.cpp
)

target_link_libraries(
        xplay
        ${ffmpeg_libs_names}
        log
        android
        GLESv3
        OpenSLES
        EGL
        xhandler
)
