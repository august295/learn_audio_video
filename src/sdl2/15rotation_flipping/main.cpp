#include <string>

#include <spdlog/spdlog.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const int SCREEN_WIDTH  = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window*   g_window   = NULL;
SDL_Renderer* g_renderer = NULL;

class LTexture
{
public:
    LTexture()
    {
        m_texture = nullptr;
        m_width   = 0;
        m_height  = 0;
    }
    ~LTexture()
    {
        free();
    }

    bool loadFromFile(std::string path)
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
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0, 0));
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
    void free()
    {
        if (m_texture != NULL)
        {
            SDL_DestroyTexture(m_texture);
            m_texture = NULL;
            m_width   = 0;
            m_height  = 0;
        }
    }
    void setColor(Uint8 red, Uint8 green, Uint8 blue)
    {
        SDL_SetTextureColorMod(m_texture, red, green, blue);
    }
    // alpha 调制
    void setBlendMode(SDL_BlendMode blending)
    {
        SDL_SetTextureBlendMode(m_texture, blending);
    }
    void setAlpha(Uint8 alpha)
    {
        SDL_SetTextureAlphaMod(m_texture, alpha);
    }
    void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE)
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

    int getWidth() { return m_width; }
    int getHeight() { return m_height; }

private:
    SDL_Texture* m_texture = nullptr;
    int          m_width   = 0;
    int          m_height  = 0;
};

// 动画
LTexture g_currentTexture;

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
            SDL_Event        e;
            bool             quit     = false;
            double           degrees  = 0.0;
            SDL_RendererFlip flipType = SDL_FLIP_NONE;
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
                        case SDLK_a: degrees -= 60; break;
                        case SDLK_d: degrees += 60; break;
                        case SDLK_q: flipType = SDL_FLIP_HORIZONTAL; break;
                        case SDLK_w: flipType = SDL_FLIP_NONE; break;
                        case SDLK_e: flipType = SDL_FLIP_VERTICAL; break;
                        }
                    }
                }

                // 清空屏幕
                SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(g_renderer);

                // 渲染图像
                g_currentTexture.render((SCREEN_WIDTH - g_currentTexture.getWidth()) / 2, (SCREEN_HEIGHT - g_currentTexture.getHeight()) / 2, NULL, degrees, NULL, flipType);

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
            }
        }
    }

    return success;
}

bool loadMedia()
{
    bool success = true;

    if (g_currentTexture.loadFromFile("resources/arrow.png") == false)
    {
        SPDLOG_ERROR("Failed to load image!");
        success = false;
    }

    return success;
}

void close()
{
    // Free image
    g_currentTexture.free();

    // Destroy window
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    g_renderer = NULL;
    g_window   = NULL;

    // Quit
    IMG_Quit();
    SDL_Quit();
}
