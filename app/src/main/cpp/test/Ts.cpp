#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../decode/VideoDecode.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif
#define MAX_AUDIO_FRAME_SIZE 192000 //48khz 16bit audio 2 channels
#define MAKE_WORD(h, l) (((h) << 8) | (l))

int main(int argc, char **argv) {

    if (argc < 2) {
        return -1;
    }
    const char *in_file = argv[1];

    AVFormatContext *fctx = NULL;
    AVCodecContext *cctx = NULL;
    AVCodec *acodec = NULL;

    FILE *audio_dst_file1 = fopen("./before_resample.pcm", "wb");
    FILE *audio_dst_file2 = fopen("./after_resample.pcm", "wb");

    av_register_all();
    avformat_open_input(&fctx, in_file, NULL, NULL);
    avformat_find_stream_info(fctx, NULL);
    //get audio index
    int aidx = av_find_best_stream(fctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    printf("get aidx[%d]!!!\n", aidx);
    //open audio codec
    AVCodecParameters *codecpar = fctx->streams[aidx]->codecpar;
    acodec = avcodec_find_decoder(codecpar->codec_id);
    cctx = avcodec_alloc_context3(acodec);
    avcodec_parameters_to_context(cctx, codecpar);
    avcodec_open2(cctx, acodec, NULL);

    //init resample
    int output_channels = 2;
    int output_rate = 48000;
    int input_channels = cctx->channels;
    int input_rate = cctx->sample_rate;
    AVSampleFormat input_sample_fmt = cctx->sample_fmt;
    AVSampleFormat output_sample_fmt = AV_SAMPLE_FMT_S16;
    printf("channels[%d=>%d],rate[%d=>%d],sample_fmt[%d=>%d]\n",
           input_channels, output_channels, input_rate, output_rate, input_sample_fmt,
           output_sample_fmt);

    SwrContext *resample_ctx = NULL;
    resample_ctx = swr_alloc_set_opts(resample_ctx,
                                      av_get_default_channel_layout(output_channels),
                                      output_sample_fmt, output_rate,
                                      av_get_default_channel_layout(input_channels),
                                      input_sample_fmt, input_rate,
                                      0, NULL);
    if (!resample_ctx) {
        printf("av_audio_resample_init fail!!!\n");
        return -1;
    }
    swr_init(resample_ctx);

    AVPacket *pkt = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    int size = 0;
    uint8_t *out_buffer = (uint8_t *) av_malloc(MAX_AUDIO_FRAME_SIZE);

    while (av_read_frame(fctx, pkt) == 0) {//DEMUX
        if (pkt->stream_index == aidx) {
            avcodec_send_packet(cctx, pkt);
            while (1) {
                int ret = avcodec_receive_frame(cctx, frame);
                if (ret != 0) {
                    break;
                } else {
                    //before resample
                    size = frame->nb_samples *
                           av_get_bytes_per_sample((AVSampleFormat) frame->format);
                    if (frame->data[0] != NULL) {
                        fwrite(frame->data[0], 1, size, audio_dst_file1);
                    }
                    //resample
                    memset(out_buffer, 0x00, sizeof(out_buffer));
                    int out_samples = swr_convert(resample_ctx, &out_buffer, frame->nb_samples,
                                                  (const uint8_t **) frame->data,
                                                  frame->nb_samples);
                    if (out_samples > 0) {
                        av_samples_get_buffer_size(NULL, output_channels, out_samples,
                                                   output_sample_fmt,
                                                   1);//out_samples*output_channels*av_get_bytes_per_sample(output_sample_fmt);
                        fwrite(out_buffer, 1, size, audio_dst_file2);
                    }
                }
                av_frame_unref(frame);
            }
        } else {
            //printf("not audio frame!!!\n");
            av_packet_unref(pkt);
            continue;
        }
        av_packet_unref(pkt);
    }

    //close
    swr_free(&resample_ctx);
    av_packet_free(&pkt);
    av_frame_free(&frame);
    avcodec_close(cctx);
    avformat_close_input(&fctx);
    av_free(out_buffer);
    fclose(audio_dst_file1);
    fclose(audio_dst_file2);

    return 0;
}

int64_t get_pts(const uint8_t *packet) {
    const uint8_t *p = packet;
    if (packet == NULL) {
        return -1;
    }

    if (!(p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01)) {    //pes sync word
        return -1;
    }
    p += 3; //jump pes sync word
    p += 4; //jump stream id(1) pes length(2) pes flag(1)

    int pts_pts_flag = *p >> 6;
    p += 2; //jump pes flag(1) pes header length(1)
    if (pts_pts_flag & 0x02) {
        int64_t pts32_30, pts29_15, pts14_0, pts;
        pts32_30 = (*p) >> 1 & 0x07;
        p += 1;
        pts29_15 = (MAKE_WORD(p[0], p[1])) >> 1;
        p += 2;
        pts14_0 = (MAKE_WORD(p[0], p[1])) >> 1;
        p += 2;
        pts = (pts32_30 << 30) | (pts29_15 << 15) | pts14_0;

        return pts;
    }
    return -1;
}

//void sync() {
//    FFDecode *video;
//    FFDecode *audio;
//    AVFrame *frame = video->getFrameQueue()->currentFrame()->frame;
//    //获取上一帧需要显示的时长delay
//    double current_pts = *(double *) frame->opaque;
//    double delay = current_pts - video->frame_last_pts;
//    if (delay <= 0 || delay >= 1.0) {
//        delay = video->frame_last_delay;
//    }
//
//    // 根据视频PTS和参考时钟调整delay
//    double ref_clock = audio->get_audio_clock();
//    double diff = current_pts - ref_clock;// diff < 0 :video slow,diff > 0 :video fast
//    //一帧视频时间或10ms，10ms音视频差距无法察觉
//    double sync_threshold = FFMAX(MIN_SYNC_THRESHOLD, FFMIN(MAX_SYNC_THRESHOLD, delay));
//
//    audio->audio_wait_video(current_pts, false);
//    video->video_drop_frame(ref_clock, false);
//    if (!isnan(diff) && fabs(diff) < NOSYNC_THRESHOLD) // 不同步
//    {
//        if (diff <= -sync_threshold)//视频比音频慢，加快
//        {
//            delay = FFMAX(0, delay + diff);
//            static int last_delay_zero_counts = 0;
//            if (video->frame_last_delay <= 0) {
//                last_delay_zero_counts++;
//            } else {
//                last_delay_zero_counts = 0;
//            }
//            if (diff < -1.0 && last_delay_zero_counts >= 10) {
//                printf("maybe video codec too slow, adjust video&audio\n");
//#ifndef DORP_PACK
//                audio->audio_wait_video(current_pts, true);//差距较大，需要反馈音频等待视频
//#endif
//                video->video_drop_frame(ref_clock, true);//差距较大，需要视频丢帧追上
//            }
//        }
//            //视频比音频快，减慢
//        else if (diff >= sync_threshold && delay > SYNC_FRAMEDUP_THRESHOLD)
//            delay = delay + diff;//音视频差距较大，且一帧的超过帧最常时间，一步到位
//        else if (diff >= sync_threshold)
//            delay = 2 * delay;//音视频差距较小，加倍延迟，逐渐缩小
//    }
//
//    video->frame_last_delay = delay;
//    video->frame_last_pts = current_pts;
//
//    double curr_time = static_cast<double>(av_gettime()) / 1000000.0;
//    if (video->frame_timer == 0) {
//        video->frame_timer = curr_time;//show first frame ,set frame timer
//    }
//
//    double actual_delay = video->frame_timer + delay - curr_time;
//    if (actual_delay <= MIN_REFRSH_S) {
//        actual_delay = MIN_REFRSH_S;
//    }
//    usleep(static_cast<int>(actual_delay * 1000 * 1000));
//    //printf("actual_delay[%lf] delay[%lf] diff[%lf]\n",actual_delay,delay,diff);
//    // Display
////        SDL_UpdateTexture(video->texture, &(video->rect), frame->data[0], frame->linesize[0]);
////        SDL_RenderClear(video->renderer);
////        SDL_RenderCopy(video->renderer, video->texture, &video->rect, &video->rect);
////        SDL_RenderPresent(video->renderer);
//    video->frame_timer = static_cast<double>(av_gettime()) / 1000000.0;
//
//    av_frame_unref(frame);
//
//    //update next frame
//    schedule_refresh(media, 1);
//
//}