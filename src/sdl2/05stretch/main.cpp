#include <string>

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window*  g_window         = NULL;
SDL_Surface* g_screenSurface  = NULL;
SDL_Surface* g_currentSurface = NULL;

// 启动 SDL 和创建窗口
bool init();
// 加载并显示图像
bool loadMedia();
// 释放资源
void close();
// 加载单个图像
SDL_Surface* loadSurface(std::string path);

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

                // 应用拉伸后的图像
                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                stretchRect.w = SCREEN_WIDTH;
                stretchRect.h = SCREEN_HEIGHT;

                SDL_BlitScaled(g_currentSurface, NULL, g_screenSurface, &stretchRect);
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

    g_currentSurface = loadSurface("resources/stretch.bmp");
    if (g_currentSurface == NULL)
    {
        spdlog::error("Failed to load stretch image!");
        success = false;
    }

    return success;
}

void close()
{
    SDL_FreeSurface(g_currentSurface);
    g_currentSurface = NULL;
    SDL_DestroyWindow(g_window);
    g_window = NULL;
    SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
    SDL_Surface* optimizedSurface = NULL;
    SDL_Surface* loadedSurface    = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL)
    {
        spdlog::error("Unable to load image {}! SDL_Error: {}", path, SDL_GetError());
    }
    else
    {
        // 将表面转换为屏幕
        optimizedSurface = SDL_ConvertSurface(loadedSurface, g_screenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            spdlog::error("Unable to optimize image {}! SDL_Error: {}", path, SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return optimizedSurface;
}