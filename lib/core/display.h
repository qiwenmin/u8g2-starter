#pragma once
#include <stdint.h>

extern "C" {
#include <clib/u8g2.h>
}

enum class FontId {
    UI12,
};

class Display {
public:
    virtual ~Display() = default;

    virtual u8g2_t* getU8g2() = 0;
};
