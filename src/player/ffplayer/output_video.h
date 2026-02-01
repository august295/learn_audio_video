#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL2/SDL.h>

#ifdef __cplusplus
}
#endif

#include "queue_av_frame.h"
#include "sync_clock.hpp"

class OutputVideo
{
public:
    OutputVideo(SyncClock* sync, AVRational time_base, QueueAVFrame* queue, int width, int height);
    ~OutputVideo();

    bool Init();
    bool MainLoop();
    void RefreshLoopWaitEvent(SDL_Event* event);
    void Refresh(double* remaining_time);

private:
    SyncClock*    m_sync;
    AVRational    m_time_base;
    QueueAVFrame* m_queue;
    int           m_width;
    int           m_height;

    SDL_Event     m_event;
    SDL_Rect      m_rect;
    SDL_Window*   m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture*  m_texture;

    std::atomic<bool> m_abort;
};