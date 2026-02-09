#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window*  g_window        = NULL;
SDL_Surface* g_screenSurface = NULL;
SDL_Surface* g_xOut          = NULL;

// 启动 SDL 和创建窗口
bool init();
// 加载并显示图像
bool loadMedia();
// 释放资源
void close();

int main(int argc, char* argv[])
{
    if (!init())
    {
        spdlog::error("Failed to initialize!");
    }
    else
    {
        if (!loadMedia())
        {
            spdlog::error("Failed to load media!");
        }
        else
        {
            SDL_Event e;
            bool      quit = false;
            while (quit == false)
            {
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }

                SDL_BlitSurface(g_xOut, NULL, g_screenSurface, NULL);
                SDL_UpdateWindowSurface(g_window);
            }
        }
    }

    close();

    return 0;
}

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        spdlog::error("SDL could not initialize! SDL_Error: {}", SDL_GetError());
        success = false;
    }
    else
    {
        g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (g_window == NULL)
        {
            spdlog::error("Window could not be created! SDL_Error: {}", SDL_GetError());
            success = false;
        }
        else
        {
            g_screenSurface = SDL_GetWindowSurface(g_window);
        }
    }

    return success;
}

bool loadMedia()
{
    bool success = true;

    g_xOut = SDL_LoadBMP("resources/x.bmp");
    if (g_xOut == NULL)
    {
        spdlog::error("Unable to load image! SDL_Error: {}", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    SDL_FreeSurface(g_screenSurface);
    g_screenSurface = NULL;
    SDL_DestroyWindow(g_window);
    g_window = NULL;
    SDL_Quit();
}