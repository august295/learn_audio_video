#include <string>

#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
        SPDLOG_ERROR("Failed to initialize!");
    }
    else
    {
        if (!loadMedia())
        {
            SPDLOG_ERROR("Failed to load media!");
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
        SPDLOG_ERROR("SDL could not initialize! SDL_Error: {}", SDL_GetError());
        success = false;
    }
    else
    {
        g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (g_window == NULL)
        {
            SPDLOG_ERROR("Window could not be created! SDL_Error: {}", SDL_GetError());
            success = false;
        }
        else
        {
            // 初始化 SDL_image
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags))
            {
                SPDLOG_ERROR("SDL_image could not initialize! SDL_image Error: {}", IMG_GetError());
                success = false;
            }
            else
            {
                g_screenSurface = SDL_GetWindowSurface(g_window);
            }
        }
    }

    return success;
}

bool loadMedia()
{
    bool success = true;

    g_currentSurface = loadSurface("resources/loaded.png");
    if (g_currentSurface == NULL)
    {
        SPDLOG_ERROR("Failed to load image!");
        success = false;
    }

    return success;
}

void close()
{
    // Free surface
    SDL_FreeSurface(g_currentSurface);
    g_currentSurface = NULL;

    // Destroy window
    SDL_DestroyWindow(g_window);
    g_window = NULL;

    // Quit
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface* loadSurface(std::string path)
{
    // SDL_image 加载图片
    SDL_Surface* loadedSurface    = IMG_Load(path.c_str());
    SDL_Surface* optimizedSurface = NULL;
    if (loadedSurface == NULL)
    {
        SPDLOG_ERROR("Unable to load image {}! SDL_Error: {}", path, IMG_GetError());
    }
    else
    {
        // 格式转换
        optimizedSurface = SDL_ConvertSurface(loadedSurface, g_screenSurface->format, 0);
        if (optimizedSurface == NULL)
        {
            SPDLOG_ERROR("Unable to optimize image {}! SDL_Error: {}", path, SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
    }
    return optimizedSurface;
}