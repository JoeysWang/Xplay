cmake_minimum_required(VERSION 3.4.1)

set(ffmpeg_libs ${CMAKE_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI})
include_directories(${CMAKE_SOURCE_DIR}/include/${CMAKE_ANDROID_ARCH_ABI})

set(ffmpeg_libs_names
        avutil avformat avcodec swscale swresample avfilter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${ffmpeg_libs}")

add_library(
        xlog
        SHARED
        XLog.cpp
)
add_library(
        cpphepler
        SHARED
        CppHelper.cpp
)


target_link_libraries(
        xlog
        log
        android
)

target_link_libraries(
        cpphepler
        log
        android
)

# Metadata库
add_subdirectory(xplayer)
add_subdirectory(metadata)
add_subdirectory(xhandler)