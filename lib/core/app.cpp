#include "app.h"
#include "display.h"

void app_render(Display& d, uint32_t tick_ms)
{
    d.clear();
    d.setFont(FontId::UI12);

    d.drawUTF8(2, 12, "U8g2 + SDL2");
    d.drawUTF8(2, 28, "Hello 你好世界");

    // 光标闪烁 demo
    if ((tick_ms / 500) & 1) {
        d.invertRect(2, 32, d.utf8Width(" "), d.lineHeight());
    }

    d.frameRect(0, 0, d.width(), d.height());
    d.update();
}
