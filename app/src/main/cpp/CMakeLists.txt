cmake_minimum_required(VERSION 3.4.1)

set(ffmpeg_libs ${CMAKE_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI})
include_directories(${CMAKE_SOURCE_DIR}/include/${CMAKE_ANDROID_ARCH_ABI})

set(ffmpeg_libs_names
        avutil avformat avcodec swscale swresample avfilter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${ffmpeg_libs}")

add_library(yuv SHARED IMPORTED )
set_target_properties( yuv
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/../../../libs/${CMAKE_ANDROID_ARCH_ABI}/libyuv.so )

add_library(
        cpphelper
        SHARED
        CppHelper.cpp
        XLog.cpp
)

target_link_libraries(
        cpphelper
        log
        android
        xhandler
        yuv
)

# Metadata库
add_subdirectory(xplayer)
add_subdirectory(metadata)
add_subdirectory(xhandler)