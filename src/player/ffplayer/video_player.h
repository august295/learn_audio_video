#pragma once

#include "thread_demux.h"
#include "thread_decode.h"
#include "queue_av_packet.h"
#include "queue_av_frame.h"
#include "output_video.h"
#include "output_audio.h"
#include "sync_clock.hpp"

class VideoPlayer
{
public:
    VideoPlayer();
    ~VideoPlayer();

    void play(const std::string& url);

public:
    QueueAVPacket m_video_packet_queue;           // 视频包队列
    QueueAVPacket m_audio_packet_queue;           // 音频包队列
    QueueAVFrame  m_video_frame_queue;            // 视频帧队列
    QueueAVFrame  m_audio_frame_queue;            // 音频帧队列
    SyncClock     m_sync_clock;                   // 同步时钟

    std::shared_ptr<ThreadDemux>  m_thread_demux; // 解复用线程
    std::shared_ptr<ThreadDecode> m_video_decode; // 视频解码线程
    std::shared_ptr<ThreadDecode> m_audio_decode; // 音频解码线程
    std::shared_ptr<OutputVideo>  m_output_video; // 视频输出
    std::shared_ptr<OutputAudio>  m_output_audio; // 音频输出
};
