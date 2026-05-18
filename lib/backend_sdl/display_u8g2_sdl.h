#pragma once

#ifdef PLATFORM_PC

#include <stdint.h>
#include <vector>

#ifdef SDL3
#include <SDL3/SDL.h>
#else
#include <SDL.h>
#endif // SDL3

extern "C" {
#include <clib/u8g2.h>
}

#include "../core/display.h"

class DisplayU8g2SDL : public Display {
public:
    DisplayU8g2SDL(int w, int h, int scale);
    ~DisplayU8g2SDL();

    u8g2_t* getU8g2() override;

    bool init();
    void pollEvents(bool& quit);

    void setScale(int scale);

private:
    int _w;
    int _h;
    int _scale;

    SDL_Window* _window = nullptr;
    SDL_Renderer* _renderer = nullptr;
    SDL_Texture* _texture = nullptr;

    u8g2_t _u8g2;

    void recreateWindow();
    void destroySDLObjects();
};

#endif
