#pragma once
#include <U8g2lib.h>
#include "../core/display.h"

class DisplayU8g2Arduino : public Display {
public:
    DisplayU8g2Arduino(U8G2& u8g2);

    u8g2_t* getU8g2() override;
private:
    U8G2& _u8g2;
};
