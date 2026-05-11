#pragma once
#include <stdint.h>

enum class FontId {
    UI12,
};

class Display {
public:
    virtual ~Display() = default;

    virtual int width() const = 0;
    virtual int height() const = 0;

    virtual void clear() = 0;

    virtual void setFont(FontId id) = 0;
    virtual int lineHeight() const = 0;

    virtual int utf8Width(const char* s) = 0;
    virtual void drawUTF8(int x, int baselineY, const char* s) = 0;

    virtual void fillRect(int x, int y, int w, int h) = 0;
    virtual void frameRect(int x, int y, int w, int h) = 0;
    virtual void invertRect(int x, int y, int w, int h) = 0;

    virtual void update() = 0;
};
