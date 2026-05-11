#include <Arduino.h>
#include <U8g2lib.h>

#include "../lib/core/app.h"
#include "../lib/backend_esp32/display_u8g2_arduino.h"

#ifndef PIN_CS
#define PIN_CS   5
#endif
#ifndef PIN_DC
#define PIN_DC   2
#endif
#ifndef PIN_RST
#define PIN_RST  4
#endif

U8G2_ST7567_OS12864_F_4W_HW_SPI u8g2(U8G2_R0, PIN_CS, PIN_DC, PIN_RST);
DisplayU8g2Arduino display(u8g2);

void setup()
{
    Serial.begin(115200);
    delay(200);

    ledcSetup(0, 5000, 8);
    ledcAttachPin(15, 0);
    ledcWrite(0, 32);

    u8g2.begin();
    u8g2.setContrast(0); // 视模块情况调整
}

void loop()
{
    uint32_t now = millis();
    app_render(display, now);
    delay(16);
}
