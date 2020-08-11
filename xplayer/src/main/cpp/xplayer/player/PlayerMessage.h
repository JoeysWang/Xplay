//
// Created by cain on 2019/1/28.
//

#ifndef PLAYERMESSAGE_H
#define PLAYERMESSAGE_H

// Message what

#define MSG_FLUSH                        00    // 默认
#define MSG_RELEASE                      -1    // 退出
#define MSG_ERROR                        10    // 出错回调
#define MSG_PREPARED                     20    // 准备完成回调
#define MSG_STARTED                      30    // 已经开始
#define MSG_COMPLETED                    40    // 播放完成回调

#define MSG_INIT                         01    // 初始化
#define MSG_SET_SURFACE_WINDOW           49    // videoView->setRenderSurface(win);
#define MSG_OPEN_INPUT                   50    // 打开文件
#define MSG_FIND_STREAM_INFO             51    // 查找媒体流信息
#define MSG_PREPARE_DECODER              52    // 准备解码器
#define MSG_VIDEO_SIZE_CHANGED           53    // 视频大小变化
#define MSG_SAR_CHANGED                  54    // 长宽比变化
#define MSG_AUDIO_START                  55    // 开始音频解码
#define MSG_AUDIO_RENDERING_START        56    // 音频渲染开始(播放开始)
#define MSG_VIDEO_START                  57    // 开始视频解码
#define MSG_VIDEO_RENDERING_START        58    // 视频渲染开始(渲染开始)
#define MSG_VIDEO_ROTATION_CHANGED       59    // 旋转角度变化

#define MSG_BUFFERING_START              60    // 缓冲开始
#define MSG_BUFFERING_END                61    // 缓冲完成
#define MSG_BUFFERING_UPDATE             62    // 缓冲更新
#define MSG_BUFFERING_TIME_UPDATE        63    // 缓冲时间更新

#define MSG_SEEK_COMPLETE                70    // 定位完成
#define MSG_PLAYBACK_STATE_CHANGED       80    // 播放状态变更
#define MSG_TIMED_TEXT                   90    // 字幕

#define MSG_REQUEST_PREPARE              200   // 异步请求准备
#define MSG_REQUEST_START                201   // 异步请求开始
#define MSG_REQUEST_PAUSE                202   // 请求暂停
#define MSG_REQUEST_SEEK                 203   // 请求定位

#define MSG_CURRENT_POSITON              300   // 当前时钟

#endif //PLAYERMESSAGE_H
