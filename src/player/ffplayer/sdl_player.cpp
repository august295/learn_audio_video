#include <spdlog/spdlog.h>

#include "sdl_player.h"

const int VIDEO_WIDTH  = 1280;
const int VIDEO_HEIGHT = 720;

int init_sdl(SDL_Context& sdlCtx, int screenWidth, int screenHeight)
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER))
    {
        SPDLOG_ERROR("Failed to init SDL");
        return -1;
    }
    sdlCtx.rect   = {0, 0, screenWidth, screenHeight};
    sdlCtx.window = SDL_CreateWindow("FFmpeg SDL2 Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, sdlCtx.rect.w, sdlCtx.rect.h, SDL_WINDOW_SHOWN);
    if (!sdlCtx.window)
    {
        SPDLOG_ERROR("Failed to create SDL window: {}", SDL_GetError());
        return -1;
    }
    sdlCtx.renderer = SDL_CreateRenderer(sdlCtx.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlCtx.renderer)
    {
        SPDLOG_ERROR("Failed to create SDL renderer: {}", SDL_GetError());
        return -1;
    }
    int ret = SDL_GetRendererInfo(sdlCtx.renderer, &sdlCtx.renderer_info);
    if (0 == ret)
    {
        SPDLOG_INFO("SDL_Renderer: {}", sdlCtx.renderer_info.name);
    }

    return 0;
}

void release_sdl(SDL_Context& sdlCtx)
{
    if (sdlCtx.renderer)
    {
        SDL_DestroyRenderer(sdlCtx.renderer);
    }
    if (sdlCtx.window)
    {
        SDL_DestroyWindow(sdlCtx.window);
    }
    SDL_Quit();
}

int run_video_player(const std::string& path)
{
    SDL_Context sdlCtx;

    if (init_sdl(sdlCtx, VIDEO_WIDTH, VIDEO_HEIGHT) < 0)
    {
        SPDLOG_ERROR("Failed to init SDL");
        return -1;
    }

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return 0;
            }
        }
    }

    release_sdl(sdlCtx);
    return 0;
}
