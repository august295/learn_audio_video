#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

#ifdef __cplusplus
}
#endif

#include "queue_av_frame.h"
#include "sync_clock.hpp"

typedef struct AudioParams
{
    int                 freq;       // 采样率
    AVChannelLayout     ch_layout;  // 声道布局(FFmpeg 7.1新特性)
    enum AVSampleFormat fmt;        // 采样格式
    int                 frame_size; // 帧大小
} AudioParams;

class OutputAudio
{
public:
    OutputAudio(SyncClock* sync, AVRational time_base, const AudioParams& audio_params, QueueAVFrame* frame_queue);
    ~OutputAudio();

    bool Init();
    void DeInit();

    static void read_audio_data(void* udata, Uint8* stream, int len);

public:
    SyncClock*    m_sync;        // 同步时钟
    AVRational    m_time_base;   // 时间基
    QueueAVFrame* m_frame_queue; // 输入帧队列
    AudioParams   m_src_params;  // 解码后的参数
    AudioParams   m_dst_params;  // SDL实际输出的格式
    SwrContext*   m_swr_ctx;     // 重采样上下文

    uint8_t* m_audio_buf;        // 重采样缓冲区
    size_t   m_audio_buf_size;   // 缓冲区大小
    size_t   m_audio_buf_index;  // 当前读取位置
    int64_t  m_pts;              // 当前帧的时间戳
};