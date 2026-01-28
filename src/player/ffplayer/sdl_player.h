#ifndef __VIDEO_PLAYER_SDL2_H__
#define __VIDEO_PLAYER_SDL2_H__

#include <string>

extern "C" {
#include <SDL2/SDL.h>
}

struct SDL_Context
{
    SDL_Rect          rect;
    SDL_Window*       window;
    SDL_Renderer*     renderer;
    SDL_RendererInfo  renderer_info;
};

/**
 * @brief   初始化 SDL
 * @param   sdlCtx                  [IN]        SDL 上下文
 * @param   screenWidth             [IN]        SDL 窗口宽度
 * @param   screenHeight            [IN]        SDL 窗口高度
 * @return  0                                   成功
 *          其他                                失败
 */
int init_sdl(SDL_Context& sdlCtx, int screenWidth, int screenHeight);

/**
 * @brief   释放 SDL
 * @param   sdlCtx                  [IN]        SDL 上下文
 * @return  0                                   成功
 *          其他                                失败
 */
void release_sdl(SDL_Context& sdlCtx);

/**
 * @brief   播放视频
 * @param   filePath                [IN]        视频文件路径
 * @return  0                                   成功
 *          其他                                失败
 */
int run_video_player(const std::string& filePath);

#endif