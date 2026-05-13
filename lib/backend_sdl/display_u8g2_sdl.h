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

    bool init();
    void pollEvents(bool& quit);

    void setScale(int scale);

    int width() const override;
    int height() const override;

    void clear() override;

    void setFont(FontId id) override;
    int lineHeight() const override;

    int utf8Width(const char* s) override;
    void drawUTF8(int x, int baselineY, const char* s) override;

    void fillRect(int x, int y, int w, int h) override;
    void frameRect(int x, int y, int w, int h) override;
    void invertRect(int x, int y, int w, int h) override;

    void update() override;

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
