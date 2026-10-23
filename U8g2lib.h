#pragma once
#include <stdint.h>
#define U8G2_R0 0
#define U8X8_PIN_NONE 255
extern const uint8_t u8g2_font_6x10_tr[], u8g2_font_logisoso24_tn[], u8g2_font_ncenB10_tr[], u8g2_font_5x8_tr[];
class U8G2_SSD1306_128X64_NONAME_1_HW_I2C{ public: U8G2_SSD1306_128X64_NONAME_1_HW_I2C(int,int); bool begin(); void firstPage(); uint8_t nextPage();
 void drawBox(int,int,int,int); void drawFrame(int,int,int,int); void drawLine(int,int,int,int); void drawCircle(int,int,int); void setDrawColor(int);
 void setFont(const uint8_t*); void drawStr(int,int,const char*); };
