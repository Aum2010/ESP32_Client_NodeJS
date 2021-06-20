#include <Wire.h>
#include "dw_font.h"
#include "SSD1306.h"
#include "string.h"

SSD1306   display(0x3c, 21, 22); //SDA - SCL

extern dw_font_info_t font_th_sarabun_new_regular14;
dw_font_t myfont;

void draw_pixel(int16_t x, int16_t y)
{
  display.setColor(WHITE);
  display.setPixel(x, y);
}

void clear_pixel(int16_t x, int16_t y)
{
  display.setColor(BLACK);
  display.setPixel(x, y);
}

void setup() {
  // put your setup code here, to run once:
  uint16_t width = 0;
 
  Serial.begin(115200);
  
  display.init();
  display.flipScreenVertically();

  dw_font_init(&myfont,
               128,
               64,
               draw_pixel,
               clear_pixel);

  dw_font_setfont(&myfont, &font_th_sarabun_new_regular14);
  dw_font_goto(&myfont, 10, 10);
  dw_font_print(&myfont, "ผมESP32!");
  display.display();

  dw_font_goto(&myfont, 10, 28);
  dw_font_print(&myfont, "ผมESP32!");
  display.display();

  dw_font_goto(&myfont, 10, 44);
  dw_font_print(&myfont, "ผมESP32!");
  display.display();
//
//  dw_font_goto(&myfont, 10, 15);
//  dw_font_print(&myfont, "ผมESP32!");
//  display.display();


}

int count;

void loop() {
  // put your main code here, to run repeatedly:
  display.resetDisplay();
  count++;
  char buff[100];
  if ( count%3 == 0 ) {
        dw_font_goto(&myfont, 10, 10);
        sprintf( buff , "%d" ,  count);
        dw_font_print(&myfont, buff);
        display.display();
    }else if( count%3 == 1 ){
        dw_font_goto(&myfont, 10, 28);
       sprintf( buff , "%d" ,  count);
        dw_font_print(&myfont, buff);
        display.display();
      }else if( count%3 == 2 ){
          dw_font_goto(&myfont, 10, 44);
       sprintf( buff , "%d" ,  count);
        dw_font_print(&myfont, buff);
        display.display();
        }
  
  delay(1000);

}
