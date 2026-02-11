#include <string>
#include <sstream>

#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window*   g_window   = NULL;
SDL_Renderer* g_renderer = NULL;
TTF_Font*     g_font     = NULL;

class LTexture
{
public:
    LTexture();
    ~LTexture();

    bool loadFromFile(std::string path);
#if defined(SDL_TTF_MAJOR_VERSION)
    // 从字体字符串创建图像
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif
    void free();
    void setColor(Uint8 red, Uint8 green, Uint8 blue);
    // alpha 调制
    void setBlendMode(SDL_BlendMode blending);
    void setAlpha(Uint8 alpha);
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    int getWidth();
    int getHeight();

private:
    SDL_Texture* m_texture = nullptr;
    int          m_width   = 0;
    int          m_height  = 0;
};

class LTimer
{
public:
    LTimer();

    void start();
    void stop();
    void pause();
    void unpause();

    Uint32 getTicks();

    bool isStarted();
    bool isPaused();

private:
    Uint32 m_startTicks;
    Uint32 m_pausedTicks;
    bool   m_paused;
    bool   m_started;
};

// 纹理
LTexture g_startPromptTexture;
LTexture g_pausePromptTexture;
LTexture g_timeTextTexture;

// 启动 SDL 和创建窗口
bool init();
// 加载并显示图像
bool loadMedia();
// 释放资源
void close();
// 加载纹理
SDL_Texture* loadTexture(std::string path);

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
            SDL_Event         e;
            bool              quit      = false;
            SDL_Color         textColor = {0, 0, 0, 255};
            LTimer            timer;
            std::stringstream timeText;
            while (quit == false)
            {
                while (SDL_PollEvent(&e))
                {
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    // 使用英文输入法
                    else if (e.type == SDL_KEYDOWN)
                    {
                        if (e.key.keysym.sym == SDLK_s)
                        {
                            if (timer.isStarted())
                            {
                                timer.stop();
                            }
                            else
                            {
                                timer.start();
                            }
                        }
                        else if (e.key.keysym.sym == SDLK_p)
                        {
                            if (timer.isPaused())
                            {
                                timer.unpause();
                            }
                            else
                            {
                                timer.pause();
                            }
                        }
                        SPDLOG_INFO("Key pressed: {}", e.key.keysym.sym);
                    }
                }

                // 清空屏幕
                SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(g_renderer);

                // 渲染图像
                timeText.str("");
                timeText << "Seconds since start time " << (timer.getTicks() / 1000.0f);
                if (!g_timeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor))
                {
                    SPDLOG_ERROR("Failed to render text.");
                }
                g_startPromptTexture.render((SCREEN_WIDTH - g_startPromptTexture.getWidth()) / 2, 0);
                g_pausePromptTexture.render((SCREEN_WIDTH - g_pausePromptTexture.getWidth()) / 2, g_startPromptTexture.getHeight());
                g_timeTextTexture.render((SCREEN_WIDTH - g_timeTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - g_timeTextTexture.getHeight()) / 2);

                // 渲染
                SDL_RenderPresent(g_renderer);
            }
        }
    }

    close();

    return 0;
}

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
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
            // 启用线性过滤
            if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            {
                SPDLOG_ERROR("Warning: Linear texture filtering not enabled!");
            }
            // 创建渲染器
            g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (g_renderer == NULL)
            {
                SPDLOG_ERROR("Renderer could not be created! SDL_Error: {}", SDL_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                // 初始化 SDL_image
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    SPDLOG_ERROR("SDL_image could not initialize! SDL_image Error: {}", IMG_GetError());
                    success = false;
                }

                // 初始化 SDL_mixer
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
                {
                    SPDLOG_ERROR("SDL_mixer could not initialize! SDL_mixer Error: {}", Mix_GetError());
                    success = false;
                }

                // 初始化 SDL_ttf
                if (TTF_Init() == -1)
                {
                    SPDLOG_ERROR("SDL_ttf could not initialize! SDL_ttf Error: {}", TTF_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    bool success = true;

    g_font = TTF_OpenFont("resources/lazy.ttf", 28);
    if (g_font == NULL)
    {
        SPDLOG_ERROR("Failed to load font! SDL_ttf Error: {}", TTF_GetError());
        success = false;
    }
    else
    {
        SDL_Color textColor = {0, 0, 0, 255};
        if (!g_startPromptTexture.loadFromRenderedText("Press S to Start or Stop the Timer", textColor))
        {
            SPDLOG_ERROR("Unable to render start/stop prompt texture!");
            success = false;
        }

        // Load pause prompt texture
        if (!g_pausePromptTexture.loadFromRenderedText("Press P to Pause or Unpause the Timer", textColor))
        {
            SPDLOG_ERROR("Unable to render pause/unpause prompt texture!");
            success = false;
        }
    }

    return success;
}

void close()
{
    // Free image
    g_startPromptTexture.free();
    g_pausePromptTexture.free();

    // Free font
    TTF_CloseFont(g_font);
    g_font = NULL;

    // Destroy window
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    g_renderer = NULL;
    g_window   = NULL;

    // Quit
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

LTexture::LTexture()
{
    m_texture = nullptr;
    m_width   = 0;
    m_height  = 0;
}

LTexture::~LTexture()
{
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    free();

    SDL_Texture* newTexture    = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL)
    {
        SPDLOG_ERROR("Unable to load image {}! SDL_Error: {}", path, IMG_GetError());
    }
    else
    {
        // 一次只能过滤一种颜色
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(g_renderer, loadedSurface);
        if (newTexture == NULL)
        {
            SPDLOG_ERROR("Unable to create texture from {}! SDL_Error: {}", path, SDL_GetError());
        }
        else
        {
            m_width  = loadedSurface->w;
            m_height = loadedSurface->h;
        }
        SDL_FreeSurface(loadedSurface);
    }
    m_texture = newTexture;
    return m_texture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
// 从字体字符串创建图像
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    free();

    // 渲染文本表面
    SDL_Surface* textSurface = TTF_RenderText_Solid(g_font, textureText.c_str(), textColor);
    if (textSurface == NULL)
    {
        SPDLOG_ERROR("Unable to render text surface! SDL_ttf Error: {}", TTF_GetError());
    }
    else
    {
        m_texture = SDL_CreateTextureFromSurface(g_renderer, textSurface);
        if (m_texture == NULL)
        {
            SPDLOG_ERROR("Unable to create texture from rendered text! SDL Error: {}", SDL_GetError());
        }
        else
        {
            m_width  = textSurface->w;
            m_height = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    return m_texture != NULL;
}
#endif

void LTexture::free()
{
    if (m_texture != NULL)
    {
        SDL_DestroyTexture(m_texture);
        m_texture = NULL;
        m_width   = 0;
        m_height  = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(m_texture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(m_texture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(m_texture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, m_width, m_height};
    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    // 渲染到屏幕
    SDL_RenderCopyEx(g_renderer, m_texture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return m_width;
}

int LTexture::getHeight()
{
    return m_height;
}

LTimer::LTimer()
{
    m_startTicks  = 0;
    m_pausedTicks = 0;
    m_paused      = false;
    m_started     = false;
}

void LTimer::start()
{
    m_started     = true;
    m_paused      = false;
    m_startTicks  = SDL_GetTicks();
    m_pausedTicks = 0;
}

void LTimer::stop()
{
    m_started     = false;
    m_paused      = false;
    m_startTicks  = 0;
    m_pausedTicks = 0;
}

void LTimer::pause()
{
    if (m_started && !m_paused)
    {
        m_paused      = true;
        m_pausedTicks = SDL_GetTicks() - m_startTicks;
        m_startTicks  = 0;
    }
}

void LTimer::unpause()
{
    if (m_started && m_paused)
    {
        m_paused      = false;
        m_startTicks  = SDL_GetTicks() - m_pausedTicks;
        m_pausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
    Uint32 time = 0;
    if (m_started)
    {
        if (m_paused)
        {
            time = m_pausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - m_startTicks;
        }
    }
    return time;
}

bool LTimer::isStarted()
{
    return m_started;
}

bool LTimer::isPaused()
{
    return m_paused && m_started;
}