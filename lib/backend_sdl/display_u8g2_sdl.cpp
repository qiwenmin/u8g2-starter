#ifdef PLATFORM_PC

#include "display_u8g2_sdl.h"

#include <stdio.h>

#define PIXEL_TO_TILE(x) ((x + 7) / 8)

static const u8x8_display_info_t u8x8_sdl_display_info = {
    /* chip_enable_level = */ 0,
    /* chip_disable_level = */ 1,

    /* post_chip_enable_wait_ns = */ 20,
    /* pre_chip_disable_wait_ns = */ 20,
    /* reset_pulse_width_ms = */ 3,
    /* post_reset_wait_ms = */ 3,
    /* sda_setup_time_ns = */ 10,
    /* sck_pulse_width_ns = */ 30,
    /* sck_clock_hz = */ 2000000UL,
    /* spi_mode = */ 0,
    /* i2c_bus_clock_100kHz = */ 4,
    /* data_setup_time_ns = */ 15,
    /* write_pulse_width_ns = */ 70,
    /* tile_width = */ (APP_WIDTH + 7) / 8,
    /* tile_height = */ (APP_HEIGHT + 7) / 8,
    /* default_x_offset = */ 0,
    /* flipmode_x_offset = */ 0,
    /* pixel_width = */ APP_WIDTH,
    /* pixel_height = */ APP_HEIGHT};


static uint8_t sdl_buf[PIXEL_TO_TILE(APP_WIDTH) * PIXEL_TO_TILE(APP_HEIGHT) * 8];
static uint32_t sdl_rgba[APP_WIDTH * APP_HEIGHT];

// SDL render context for passing to u8g2 callback
struct SDLRenderContext {
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
};

static SDLRenderContext* g_sdlContext = nullptr;

static uint8_t u8x8_d_sdl(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    switch (msg)
    {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
        u8x8_d_helper_display_setup_memory(u8x8, &u8x8_sdl_display_info);
        break;

    case U8X8_MSG_DISPLAY_INIT:
        u8x8_d_helper_display_init(u8x8);
        break;

    case U8X8_MSG_DISPLAY_DRAW_TILE:
        {
            u8x8_tile_t *tile = (u8x8_tile_t *)arg_ptr;
            int x_start = tile->x_pos * 8;
            int y_start = tile->y_pos * 8;
            uint8_t *data = tile->tile_ptr;
            uint8_t cnt = tile->cnt;

            for (int i = 0; i < cnt; i++) {
                for (int tx = 0; tx < 8; tx++) {
                    int x = x_start + (i * 8) + tx;
                    if (x >= APP_WIDTH) break;

                    uint8_t pixel_col = data[i * 8 + tx];

                    for (int ty = 0; ty < 8; ty++) {
                        int y = y_start + ty;
                        if (y >= APP_HEIGHT) break;

                        bool is_on = (pixel_col >> ty) & 0x01;
                        sdl_rgba[APP_WIDTH * y + x] = is_on ? 0xFF000000 : 0xFFFFFFFF;
                    }
                }
            }

            SDLRenderContext* ctx = (SDLRenderContext*)u8x8_GetUserPtr(u8x8);
            if (ctx && ctx->renderer && ctx->texture) {
                int tile_x = x_start;
                int tile_y = y_start;
                int tile_w = cnt * 8;
                if (tile_x + tile_w > ctx->width) {
                    tile_w = ctx->width - tile_x;
                }
                if (tile_w > 0) {
                    SDL_Rect rect{tile_x, tile_y, tile_w, 8};
                    SDL_UpdateTexture(
                        ctx->texture,
                        &rect,
                        &sdl_rgba[ctx->width * tile_y + tile_x],
                        ctx->width * sizeof(uint32_t)
                    );
                }
            }
        }
        break;

    case U8X8_MSG_DISPLAY_REFRESH:
        {
            SDLRenderContext* ctx = (SDLRenderContext*)u8x8_GetUserPtr(u8x8);
            if (ctx && ctx->renderer && ctx->texture) {
                // 1. 清空画布
                SDL_RenderClear(ctx->renderer);
                // 2. 将整张已经更新完毕的纹理复制到后台缓冲区
#ifdef SDL3
                SDL_RenderTexture(ctx->renderer, ctx->texture, nullptr, nullptr);
#else
                SDL_RenderCopy(ctx->renderer, ctx->texture, nullptr, nullptr);
#endif // SDL3
                // 3. 真正刷新到屏幕上（整帧一次性呈现，防止撕裂闪烁）
                SDL_RenderPresent(ctx->renderer);
            }
        }
        break;
    default:
        return 0;
    }
    return 1;
}

// dummy callback: no real SPI/I2C needed
static uint8_t u8x8_byte_dummy(u8x8_t*, uint8_t, uint8_t, void*)
{
    return 0;
}

// dummy callback: no GPIO/delay needed
static uint8_t u8x8_gpio_and_delay_dummy(u8x8_t*, uint8_t, uint8_t, void*)
{
    return 0;
}

DisplayU8g2SDL::DisplayU8g2SDL(int w, int h, int scale)
    : _w(w), _h(h), _scale(scale)
{
    if (_scale < 1) _scale = 1;
}

DisplayU8g2SDL::~DisplayU8g2SDL()
{
    destroySDLObjects();
    SDL_Quit();
}

void DisplayU8g2SDL::destroySDLObjects()
{
    if (_texture) {
        SDL_DestroyTexture(_texture);
        _texture = nullptr;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = nullptr;
    }
    if (g_sdlContext) {
        delete g_sdlContext;
        g_sdlContext = nullptr;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

u8g2_t* DisplayU8g2SDL::getU8g2()
{
    return &_u8g2;
}

void DisplayU8g2SDL::createWindow()
{
    destroySDLObjects();

#ifdef SDL3
    _window = SDL_CreateWindow(
        "U8g2 SDL Simulator",
        _w * _scale,
        _h * _scale,
        0
    );

    _renderer = SDL_CreateRenderer(
        _window,
        nullptr
    );
#else
    _window = SDL_CreateWindow(
        "U8g2 SDL Simulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _w * _scale,
        _h * _scale,
        SDL_WINDOW_SHOWN
    );

    _renderer = SDL_CreateRenderer(
        _window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
#endif // SDL3

    _texture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        _w,
        _h
    );

#ifdef SDL3
    SDL_SetTextureScaleMode(_texture, SDL_SCALEMODE_NEAREST);
#endif // SDL3

    if (g_sdlContext) {
        g_sdlContext->renderer = _renderer;
        g_sdlContext->texture = _texture;
    }
}

bool DisplayU8g2SDL::init()
{
#ifdef SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
#endif // SDL3
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

#ifndef SDL3
    // nearest neighbor scaling (pixel perfect)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
#endif // !SDL3

    createWindow();

    int page_count = (_h + 7) / 8;
    u8g2_SetupDisplay(&_u8g2, u8x8_d_sdl, u8x8_cad_001, u8x8_byte_dummy, u8x8_gpio_and_delay_dummy);
    u8g2_SetupBuffer(&_u8g2, sdl_buf, page_count, u8g2_ll_hvline_vertical_top_lsb, U8G2_R0);

    g_sdlContext = new SDLRenderContext{
        .renderer = _renderer,
        .texture = _texture,
        .width = _w
    };
    u8x8_SetUserPtr(&(_u8g2.u8x8), g_sdlContext);

    u8g2_InitDisplay(&_u8g2);
    u8g2_SetPowerSave(&_u8g2, 0);
    u8g2_ClearBuffer(&_u8g2);

    return true;
}

void DisplayU8g2SDL::pollEvents(bool& quit)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
#ifdef SDL3
        if (e.type == SDL_EVENT_QUIT) {
#else
        if (e.type == SDL_QUIT) {
#endif // SDL3
            quit = true;
#ifdef SDL3
        } else if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.key) {
#else
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
#endif // SDL3
            case SDLK_ESCAPE:
                quit = true;
                break;
            case SDLK_F2:
                setScale(2);
                break;
            case SDLK_F3:
                setScale(3);
                break;
            default:
                break;
            }
        }
    }
}

void DisplayU8g2SDL::setScale(int scale)
{
    if (scale < 2) scale = 2;
    if (scale == _scale) return;

    _scale = scale;
    SDL_SetWindowSize(_window, _w * _scale, _h * _scale);
}

#endif
