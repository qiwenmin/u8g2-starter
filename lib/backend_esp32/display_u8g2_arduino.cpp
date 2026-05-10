#include "display_u8g2_arduino.h"

DisplayU8g2Arduino::DisplayU8g2Arduino(U8G2& u8g2)
    : _u8g2(u8g2)
{
}

int DisplayU8g2Arduino::width() const { return _u8g2.getDisplayWidth(); }
int DisplayU8g2Arduino::height() const { return _u8g2.getDisplayHeight(); }

void DisplayU8g2Arduino::clear()
{
    _u8g2.clearBuffer();
}

void DisplayU8g2Arduino::setFont(FontId id)
{
    switch (id) {
    case FontId::UI12:
        // 先用自带英文小字体跑通
        // 后面你换成 u8g2_font_wqy12_t_gb2312 等中文字体
        _u8g2.setFont(u8g2_font_6x12_tf);
        break;
    }
}

int DisplayU8g2Arduino::lineHeight() const
{
    return _u8g2.getMaxCharHeight();
}

int DisplayU8g2Arduino::utf8Width(const char* s)
{
    return _u8g2.getUTF8Width(s);
}

void DisplayU8g2Arduino::drawUTF8(int x, int baselineY, const char* s)
{
    _u8g2.drawUTF8(x, baselineY, s);
}

void DisplayU8g2Arduino::fillRect(int x, int y, int w, int h)
{
    _u8g2.drawBox(x, y, w, h);
}

void DisplayU8g2Arduino::frameRect(int x, int y, int w, int h)
{
    _u8g2.drawFrame(x, y, w, h);
}

void DisplayU8g2Arduino::invertRect(int x, int y, int w, int h)
{
    _u8g2.setDrawColor(2); // XOR
    _u8g2.drawBox(x, y, w, h);
    _u8g2.setDrawColor(1);
}

void DisplayU8g2Arduino::update()
{
    _u8g2.sendBuffer();
}
