#include <spdlog/spdlog.h>

#include "video_player.h"

VideoPlayer::VideoPlayer()
{
    m_video_packet_queue.SetName("video_packet_queue");
    m_audio_packet_queue.SetName("audio_packet_queue");
    m_video_frame_queue.SetName("video_frame_queue");
    m_audio_frame_queue.SetName("audio_frame_queue");
}

VideoPlayer::~VideoPlayer()
{
    spdlog::info("~VideoPlayer");
}

void VideoPlayer::play(const std::string& url)
{
    // 1. 解复用
    // TODO: 这里处理视频+音频，可以扩展字幕
    m_thread_demux = std::make_shared<ThreadDemux>(&m_video_packet_queue, &m_audio_packet_queue);
    if (false == m_thread_demux->Open(url))
    {
        return;
    }
    m_thread_demux->Start();

    // 2. 解码
    // 2.1 解码视频
    m_video_decode = std::make_shared<ThreadDecode>(&m_video_packet_queue, &m_video_frame_queue);
    if (false == m_video_decode->Init(m_thread_demux->VideoCodecParameters()))
    {
        return;
    }
    m_video_decode->Start();

    // 2.2 解码音频
    m_audio_decode = std::make_shared<ThreadDecode>(&m_audio_packet_queue, &m_audio_frame_queue);
    if (false == m_audio_decode->Init(m_thread_demux->AudioCodecParameters()))
    {
        return;
    }
    m_audio_decode->Start();

    // 3. 播放
    // 3.1 播放音频
    AudioParams audio_params;
    memset(&audio_params, 0, sizeof(AudioParams));
    audio_params.ch_layout  = m_thread_demux->AudioCodecParameters()->ch_layout;
    audio_params.fmt        = (enum AVSampleFormat)m_thread_demux->AudioCodecParameters()->format;
    audio_params.freq       = m_thread_demux->AudioCodecParameters()->sample_rate;
    audio_params.frame_size = m_thread_demux->AudioCodecParameters()->frame_size;
    m_output_audio          = std::make_shared<OutputAudio>(&m_sync_clock, m_thread_demux->AudioStreamTimebase(), audio_params, &m_audio_frame_queue);
    if (false == m_output_audio->Init())
    {
        return;
    }

    // 3.2 播放视频
    m_output_video = std::make_shared<OutputVideo>(&m_sync_clock, m_thread_demux->VideoStreamTimebase(), &m_video_frame_queue, m_thread_demux->VideoCodecParameters()->width, m_thread_demux->VideoCodecParameters()->height);
    if (false == m_output_video->Init())
    {
        return;
    }
    m_output_video->MainLoop();
}