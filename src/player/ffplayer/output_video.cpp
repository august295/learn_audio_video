#include <spdlog/spdlog.h>

#include "output_video.h"

OutputVideo::OutputVideo(SyncClock* sync, AVRational time_base, QueueAVFrame* queue, int width, int height)
    : m_sync(sync)
    , m_time_base(time_base)
    , m_queue(queue)
    , m_width(width)
    , m_height(height)
{
    m_event    = {};
    m_rect     = {0, 0, m_width, m_height};
    m_window   = nullptr;
    m_renderer = nullptr;
    m_texture  = nullptr;
    m_abort.store(false);
}

OutputVideo::~OutputVideo()
{
    spdlog::info("~OutputVideo");
    if (m_texture)
    {
        SDL_DestroyTexture(m_texture);
    }
    if (m_renderer)
    {
        SDL_DestroyRenderer(m_renderer);
    }
    if (m_window)
    {
        SDL_DestroyWindow(m_window);
    }
}

bool OutputVideo::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        spdlog::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }
    m_window = SDL_CreateWindow("FFmpeg Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, 0);
    if (!m_window)
    {
        spdlog::error("Failed to create window: {}", SDL_GetError());
        return false;
    }
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if (!m_renderer)
    {
        spdlog::error("Failed to create renderer: {}", SDL_GetError());
        return false;
    }
    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
    if (!m_texture)
    {
        spdlog::error("Failed to create texture: {}", SDL_GetError());
        return false;
    }
    return true;
}

bool OutputVideo::MainLoop()
{
    SDL_Event event;
    while (true)
    {
        // 读取事件
        RefreshLoopWaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                spdlog::info("SDLK_ESCAPE");
                return false;
            }
            break;
        case SDL_QUIT:
            spdlog::info("SDL_QUIT");
            return false;
        default:
            break;
        }
    }
    return true;
}

void OutputVideo::RefreshLoopWaitEvent(SDL_Event* event)
{
    double remaining_time = 0.0;
    SDL_PumpEvents();
    while (!SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) && !m_abort.load())
    {
        if (remaining_time > 0.0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(remaining_time * 1000.0)));
        }
        remaining_time = 0.01;
        // 尝试刷新画面
        Refresh(&remaining_time);
        SDL_PumpEvents();
    }
}

void OutputVideo::Refresh(double* remaining_time)
{
    AVFrame* frame = nullptr;
    // 获取当前帧，不弹出队列，确认使用再弹出
    m_queue->Front(frame);
    if (frame)
    {
        double video_pts   = frame->pts * av_q2d(m_time_base);
        double audio_clock = m_sync->GetClock();
        double diff        = video_pts - audio_clock;
        spdlog::debug("video pts: {}", video_pts);

        double sync_threshold = 0.04; // 40ms 同步阈值
        // 时钟同步
        if (diff > sync_threshold)
        {
            // 需要等待
            *remaining_time = FFMIN(*remaining_time, diff);
            return;
        }
        else if (diff < -sync_threshold)
        {
            // 需要丢帧
            spdlog::debug("drop: {}", video_pts);
        }
        else
        {
            // 更新YUV纹理
            SDL_UpdateYUVTexture(m_texture, &m_rect, frame->data[0], frame->linesize[0], frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
            SDL_RenderClear(m_renderer);
            SDL_RenderCopy(m_renderer, m_texture, nullptr, &m_rect);
            SDL_RenderPresent(m_renderer);
        }
        // 移除帧
        m_queue->Pop(frame);
        av_frame_free(&frame);
    }
    else
    {
        if (m_queue->m_abort.load())
        {
            // 视频流结束
            m_abort.store(true);
            spdlog::info("End of file: output video");
        }
    }
}