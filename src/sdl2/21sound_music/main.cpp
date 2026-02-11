#include <string>

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

// 纹理
LTexture   g_currentTexture;
Mix_Music* g_music   = NULL;
Mix_Chunk* g_scratch = NULL;
Mix_Chunk* g_high    = NULL;
Mix_Chunk* g_medium  = NULL;
Mix_Chunk* g_low     = NULL;

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
                        case SDLK_1: Mix_PlayChannel(-1, g_high, 0); break;
                        case SDLK_2: Mix_PlayChannel(-1, g_medium, 0); break;
                        case SDLK_3: Mix_PlayChannel(-1, g_low, 0); break;
                        case SDLK_4: Mix_PlayChannel(-1, g_scratch, 0); break;
                        case SDLK_9:
                            if (Mix_PlayingMusic() == 0)
                            {
                                Mix_PlayMusic(g_music, -1);
                            }
                            else
                            {
                                if (Mix_PausedMusic() == 1)
                                {
                                    Mix_ResumeMusic();
                                }
                                else
                                {
                                    Mix_PauseMusic();
                                }
                            }
                            break;
                        case SDLK_0: Mix_HaltMusic(); break;
                        }
                        SPDLOG_INFO("Key pressed: {}", e.key.keysym.sym);
                    }
                }

                // 清空屏幕
                SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(g_renderer);

                // 渲染图像
                g_currentTexture.render(0, 0);

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

    if (g_currentTexture.loadFromFile("resources/prompt.png") == false)
    {
        SPDLOG_ERROR("Failed to load image!");
        success = false;
    }

    g_music = Mix_LoadMUS("resources/beat.wav");
    if (g_music == NULL)
    {
        SPDLOG_ERROR("Failed to load beat.wav! SDL_mixer Error: {}", Mix_GetError());
        success = false;
    }

    g_scratch = Mix_LoadWAV("resources/scratch.wav");
    if (g_scratch == NULL)
    {
        SPDLOG_ERROR("Failed to load scratch.wav! SDL_mixer Error: {}", Mix_GetError());
        success = false;
    }
    g_high = Mix_LoadWAV("resources/high.wav");
    if (g_high == NULL)
    {
        SPDLOG_ERROR("Failed to load high.wav! SDL_mixer Error: {}", Mix_GetError());
        success = false;
    }
    g_medium = Mix_LoadWAV("resources/medium.wav");
    if (g_medium == NULL)
    {
        SPDLOG_ERROR("Failed to load medium.wav! SDL_mixer Error: {}", Mix_GetError());
        success = false;
    }
    g_low = Mix_LoadWAV("resources/low.wav");
    if (g_low == NULL)
    {
        SPDLOG_ERROR("Failed to load low.wav! SDL_mixer Error: {}", Mix_GetError());
        success = false;
    }

    return success;
}

void close()
{
    // Free image
    g_currentTexture.free();

    // Free music
    Mix_FreeChunk(g_scratch);
    g_scratch = NULL;
    Mix_FreeChunk(g_high);
    g_high = NULL;
    Mix_FreeChunk(g_medium);
    g_medium = NULL;
    Mix_FreeChunk(g_low);
    g_low = NULL;
    Mix_FreeMusic(g_music);
    g_music = NULL;

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