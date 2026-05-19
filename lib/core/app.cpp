#include "app.h"
#include "display.h"

void app_render(Display& d, uint32_t tick_ms)
{
    u8g2_t *u8g2 = d.getU8g2();

    u8g2_ClearBuffer(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_6x12_tf);
    u8g2_DrawUTF8(u8g2, 2, 12, "U8g2 + SDL2");

    u8g2_SetFont(u8g2, u8g2_font_wqy12_t_chinese1);
    u8g2_DrawUTF8(u8g2, 2, 28, "Hello 你好世界");

    // 光标闪烁 demo
    if ((tick_ms / 500) & 1) {
        u8g2_SetDrawColor(u8g2, 2); // XOR
        u8g2_DrawBox(u8g2, 2, 32, u8g2_GetUTF8Width(u8g2, " "), u8g2_GetMaxCharHeight(u8g2));
        u8g2_SetDrawColor(u8g2, 1);
    }

    u8g2_DrawFrame(u8g2, 0, 0, APP_WIDTH, APP_HEIGHT);
    u8g2_SendBuffer(u8g2);
}
