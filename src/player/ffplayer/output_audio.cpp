#include <spdlog/spdlog.h>

#include "output_audio.h"

OutputAudio::OutputAudio(SyncClock* sync, AVRational time_base, const AudioParams& audio_params, QueueAVFrame* frame_queue)
    : m_sync(sync)
    , m_time_base(time_base)
    , m_frame_queue(frame_queue)
    , m_src_params(audio_params)
{
    m_dst_params      = {0};
    m_swr_ctx         = nullptr;
    m_audio_buf       = nullptr;
    m_audio_buf_size  = 0;
    m_audio_buf_index = 0;
    m_pts             = AV_NOPTS_VALUE;
}

OutputAudio::~OutputAudio()
{
    spdlog::info("~OutputAudio");
    DeInit();
}

void OutputAudio::read_audio_data(void* udata, Uint8* stream, int len)
{
    int          ret   = 0;
    OutputAudio* audio = static_cast<OutputAudio*>(udata);

    while (len > 0)
    {
        if (audio->m_audio_buf_index == audio->m_audio_buf_size)
        {
            audio->m_audio_buf_index = 0;
            AVFrame* frame           = nullptr;
            audio->m_frame_queue->Pop(frame, std::chrono::milliseconds(100));
            if (frame)
            {
                audio->m_pts = frame->pts;
                // 检查是否需要重采样
                if (((frame->format != audio->m_dst_params.fmt) ||
                     (frame->sample_rate != audio->m_dst_params.freq) ||
                     (av_channel_layout_compare(&frame->ch_layout, &audio->m_dst_params.ch_layout))) &&
                    (audio->m_swr_ctx == nullptr))
                {
                    // 创建重采样上下文
                    ret = swr_alloc_set_opts2(&audio->m_swr_ctx,
                                              &audio->m_dst_params.ch_layout,
                                              audio->m_dst_params.fmt,
                                              audio->m_dst_params.freq,
                                              &frame->ch_layout,
                                              (enum AVSampleFormat)frame->format,
                                              frame->sample_rate,
                                              0,
                                              nullptr);
                    if (audio->m_swr_ctx == nullptr || swr_init(audio->m_swr_ctx) < 0)
                    {
                        spdlog::error("Failed to initialize swr context");
                        swr_free(&audio->m_swr_ctx);
                        return;
                    }
                }
                if (audio->m_swr_ctx)
                {
                    // 重采样处理
                    uint8_t** in          = frame->extended_data;
                    int       out_samples = (frame->nb_samples * audio->m_dst_params.freq / frame->sample_rate) + 256;
                    int       out_len     = av_samples_get_buffer_size(nullptr, audio->m_dst_params.ch_layout.nb_channels, out_samples, audio->m_dst_params.fmt, 0);
                    if (out_len < 0)
                    {
                        spdlog::error("Failed to get output buffer size");
                        return;
                    }
                    av_fast_malloc(&audio->m_audio_buf, (unsigned int*)&audio->m_audio_buf_size, out_len);
                    int convert_len = swr_convert(audio->m_swr_ctx, &audio->m_audio_buf, out_samples, in, frame->nb_samples);
                    if (convert_len < 0)
                    {
                        spdlog::error("Failed to convert audio data");
                        return;
                    }
                    audio->m_audio_buf_size = av_samples_get_buffer_size(nullptr, audio->m_dst_params.ch_layout.nb_channels, convert_len, audio->m_dst_params.fmt, 1);
                }
                else
                {
                    // 直接使用原始数据
                    int temp = av_samples_get_buffer_size(nullptr, frame->ch_layout.nb_channels, frame->nb_samples, (enum AVSampleFormat)frame->format, 1);
                    av_fast_malloc(&audio->m_audio_buf, (unsigned int*)&audio->m_audio_buf_size, temp);
                    audio->m_audio_buf_size = temp;
                    memcpy(audio->m_audio_buf, frame->data[0], audio->m_audio_buf_size);
                }
                av_frame_free(&frame);
            }
            else
            {
                audio->m_audio_buf      = nullptr;
                audio->m_audio_buf_size = 512;
            }
        }

        int cost_len = audio->m_audio_buf_size - audio->m_audio_buf_index;
        if (cost_len > len)
        {
            cost_len = len;
        }
        // 拷贝数据到SDL音频流
        if (audio->m_audio_buf == nullptr)
        {
            memset(stream, 0, cost_len);
        }
        else
        {
            memcpy(stream, audio->m_audio_buf + audio->m_audio_buf_index, cost_len);
        }
        len -= cost_len;
        stream += cost_len;
        audio->m_audio_buf_index += cost_len;
    }

    // 设置音频时钟
    if (audio->m_pts != AV_NOPTS_VALUE)
    {
        double pts = audio->m_pts * av_q2d(audio->m_time_base);
        audio->m_sync->SetClock(pts);
        spdlog::debug("audio pts: {}", pts);
    }
}

bool OutputAudio::Init()
{
    int ret = 0;
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }

    // 设置输出音频参数
    SDL_AudioSpec wanted_spec;
    SDL_zero(wanted_spec);
    wanted_spec.freq     = m_src_params.freq;
    wanted_spec.format   = AUDIO_S16SYS;
    wanted_spec.channels = 2;
    wanted_spec.silence  = 0;
    wanted_spec.samples  = 1024 * 2;
    wanted_spec.callback = OutputAudio::read_audio_data;
    wanted_spec.userdata = this;

    ret = SDL_OpenAudio(&wanted_spec, nullptr);
    if (ret != 0)
    {
        spdlog::error("Failed to open audio: {}", SDL_GetError());
        return false;
    }
    av_channel_layout_default(&m_dst_params.ch_layout, wanted_spec.channels);
    m_dst_params.fmt        = AV_SAMPLE_FMT_S16;
    m_dst_params.freq       = wanted_spec.freq;
    m_dst_params.frame_size = wanted_spec.samples;
    SDL_PauseAudio(0);

    return true;
}

void OutputAudio::DeInit()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();

    // 释放重采样上下文
    if (m_swr_ctx)
    {
        swr_free(&m_swr_ctx);
    }
}