#include "U8g2lib.h"
#include <cstdio>
const uint8_t u8g2_font_6x10_tr[1]={0}, u8g2_font_logisoso24_tn[1]={0}, u8g2_font_ncenB10_tr[1]={0}, u8g2_font_5x8_tr[1]={0};
static int pg=0;
U8G2_SSD1306_128X64_NONAME_1_HW_I2C::U8G2_SSD1306_128X64_NONAME_1_HW_I2C(int,int){} bool U8G2_SSD1306_128X64_NONAME_1_HW_I2C::begin(){return true;}
void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::firstPage(){pg=0;} uint8_t U8G2_SSD1306_128X64_NONAME_1_HW_I2C::nextPage(){return ++pg<8;}
void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::drawBox(int,int,int,int){} void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::drawFrame(int,int,int,int){}
void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::drawLine(int x0,int y0,int x1,int y1){ if(x0<0||x1>127||y0<43||y1>63||y0>63||y1<43) printf("BAD LINE %d,%d %d,%d\n",x0,y0,x1,y1);}
void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::drawCircle(int,int,int){} void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::setDrawColor(int){}
void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::setFont(const uint8_t*){} void U8G2_SSD1306_128X64_NONAME_1_HW_I2C::drawStr(int,int,const char*){}
