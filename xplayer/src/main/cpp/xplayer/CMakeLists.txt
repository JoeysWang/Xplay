
include_directories(../xhandler)
add_library(
        xplayer

        SHARED

        player_android.cpp

        FFPlayer.cpp
        demux/Demuxer.cpp

        decode/AudioDecode.cpp
        decode/VideoDecode.cpp
        decode/IDecode.cpp

        player/MediaSync.cpp
        player/VideoPlayer.cpp
        player/AudioPlayer.cpp

        data/PlayerState.cpp
        data/DecodeParam.cpp
        data/PacketData.cpp
        data/FrameData.cpp

        dataStructure/Queue.cpp

)

target_link_libraries(
        xplayer
        ${ffmpeg_libs_names}
        log
        android
        GLESv3
        OpenSLES
        EGL
        xhandler
)
