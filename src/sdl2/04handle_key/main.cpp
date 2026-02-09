#include <string>

#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

// 按键枚举
enum KeyPressSurface
{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL,
};

SDL_Window*  g_window         = NULL;
SDL_Surface* g_screenSurface  = NULL;
SDL_Surface* g_currentSurface = NULL;

SDL_Surface* g_keyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

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
            g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
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
                    else if (e.type == SDL_KEYDOWN)
                    {
                        switch (e.key.keysym.sym)
                        {
                        case SDLK_UP:    g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_UP]; break;
                        case SDLK_DOWN:  g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_DOWN]; break;
                        case SDLK_LEFT:  g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_LEFT]; break;
                        case SDLK_RIGHT: g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_RIGHT]; break;
                        default:         g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT]; break;
                        }
                    }
                }

                SDL_BlitSurface(g_currentSurface, NULL, g_screenSurface, NULL);
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

    // 默认图像
    g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("resources/press.bmp");
    if (g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
    {
        spdlog::error("Failed to load default image!");
        success = false;
    }

    // 上
    g_keyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("resources/up.bmp");
    if (g_keyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
    {
        spdlog::error("Failed to load up image!");
        success = false;
    }

    // 下
    g_keyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("resources/down.bmp");
    if (g_keyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
    {
        spdlog::error("Failed to load down image!");
        success = false;
    }

    // 左
    g_keyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("resources/left.bmp");
    if (g_keyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
    {
        spdlog::error("Failed to load left image!");
        success = false;
    }

    // 右
    g_keyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("resources/right.bmp");
    if (g_keyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
    {
        spdlog::error("Failed to load right image!");
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

SDL_Surface* loadSurface(std::string path)
{
    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL)
    {
        spdlog::error("Unable to load image! SDL_Error: {}", SDL_GetError());
    }
    return loadedSurface;
}