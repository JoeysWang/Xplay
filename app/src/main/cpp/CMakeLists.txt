cmake_minimum_required(VERSION 3.4.1)


set(ffmpeg_libs ${CMAKE_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI})
include_directories(${CMAKE_SOURCE_DIR}/include/${CMAKE_ANDROID_ARCH_ABI})

set(ffmpeg_libs_names
        avutil avformat avcodec swscale swresample avfilter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${ffmpeg_libs}")

add_library(
        xplay
        SHARED
        xplay.cpp
        demux/IDemux.cpp
        demux/FFDemux.cpp
        data/XData.cpp
        data/XParameter.cpp
        XLog.cpp
        XThread.cpp
        IObserver.cpp
        decode/IDecode.cpp
        decode/FFDecode.cpp
)

#find_library(
##        log-lib
##        log)

target_link_libraries(
        xplay
        ${ffmpeg_libs_names}
        log
        android
        GLESv2
        OpenSLES
        EGL
)
