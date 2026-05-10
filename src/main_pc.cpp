#ifdef PLATFORM_PC

#include <stdint.h>
#include <chrono>

#include "../lib/core/app.h"
#include "../lib/backend_sdl/display_u8g2_sdl.h"

static uint32_t millis_now()
{
    using namespace std::chrono;
    auto ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
    return (uint32_t)ms.count();
}

int main()
{
    DisplayU8g2SDL display(APP_WIDTH, APP_HEIGHT, 2);

    if (!display.init()) {
        return -1;
    }

    bool quit = false;

    while (!quit) {
        display.pollEvents(quit);

        uint32_t now = millis_now();
        app_render(display, now);

        SDL_Delay(16);
    }

    return 0;
}

#endif
