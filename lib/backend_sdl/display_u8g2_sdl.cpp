#ifdef PLATFORM_PC

#include "display_u8g2_sdl.h"

#include <stdio.h>

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
    _rgba.resize(_w * _h);
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
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = nullptr;
    }
}

void DisplayU8g2SDL::recreateWindow()
{
    destroySDLObjects();

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

    _texture = SDL_CreateTexture(
        _renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        _w,
        _h
    );
}

bool DisplayU8g2SDL::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    // nearest neighbor scaling (pixel perfect)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    recreateWindow();

    // Use official u8g2 setup function (same as your real screen)
    // Full-buffer mode (_f)
    u8g2_Setup_st7567_os12864_f(
        &_u8g2,
        U8G2_R0,
        u8x8_byte_dummy,
        u8x8_gpio_and_delay_dummy
    );

    u8g2_InitDisplay(&_u8g2);
    u8g2_SetPowerSave(&_u8g2, 0);
    u8g2_ClearBuffer(&_u8g2);

    return true;
}

void DisplayU8g2SDL::pollEvents(bool& quit)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
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
    if (scale < 1) scale = 1;
    if (scale == _scale) return;

    _scale = scale;
    recreateWindow();
}

int DisplayU8g2SDL::width() const
{
    return _w;
}

int DisplayU8g2SDL::height() const
{
    return _h;
}

void DisplayU8g2SDL::clear()
{
    u8g2_ClearBuffer(&_u8g2);
}

void DisplayU8g2SDL::setFont(FontId id)
{
    switch (id) {
    case FontId::UI12:
        u8g2_SetFont(&_u8g2, u8g2_font_6x12_tf);
        break;
    }
}

int DisplayU8g2SDL::lineHeight() const
{
    return u8g2_GetMaxCharHeight(&_u8g2);
}

int DisplayU8g2SDL::utf8Width(const char* s)
{
    return u8g2_GetUTF8Width(&_u8g2, s);
}

void DisplayU8g2SDL::drawUTF8(int x, int baselineY, const char* s)
{
    u8g2_DrawUTF8(&_u8g2, x, baselineY, s);
}

void DisplayU8g2SDL::fillRect(int x, int y, int w, int h)
{
    u8g2_DrawBox(&_u8g2, x, y, w, h);
}

void DisplayU8g2SDL::frameRect(int x, int y, int w, int h)
{
    u8g2_DrawFrame(&_u8g2, x, y, w, h);
}

void DisplayU8g2SDL::invertRect(int x, int y, int w, int h)
{
    u8g2_SetDrawColor(&_u8g2, 2); // XOR
    u8g2_DrawBox(&_u8g2, x, y, w, h);
    u8g2_SetDrawColor(&_u8g2, 1);
}

void DisplayU8g2SDL::update()
{
    uint8_t *buf = u8g2_GetBufferPtr(&_u8g2);


    for (int y = 0; y < _h; y++) {
        for (int x = 0; x < _w; x++) {

            uint8_t pixel = (buf[(y >> 3) * APP_WIDTH + x] >> (y & 7)) & 1;

            _rgba[y * _w + x] =
                pixel ? 0xFF000000 : 0xFFFFFFFF;
        }
    }

    SDL_UpdateTexture(
        _texture,
        nullptr,
        _rgba.data(),
        _w * sizeof(uint32_t)
    );

    SDL_RenderClear(_renderer);

    SDL_Rect dst{0, 0, _w * _scale, _h * _scale};
    SDL_RenderCopy(_renderer, _texture, nullptr, &dst);

    SDL_RenderPresent(_renderer);
}

#endif
