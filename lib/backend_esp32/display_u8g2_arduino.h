#pragma once
#include <U8g2lib.h>
#include "../core/display.h"

class DisplayU8g2Arduino : public Display {
public:
    DisplayU8g2Arduino(U8G2& u8g2);

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
    U8G2& _u8g2;
};
