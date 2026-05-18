#include "display_u8g2_arduino.h"

DisplayU8g2Arduino::DisplayU8g2Arduino(U8G2& u8g2)
    : _u8g2(u8g2)
{
}

u8g2_t* DisplayU8g2Arduino::getU8g2()
{
    return _u8g2.getU8g2();
}
