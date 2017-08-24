#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <power.h>

#define RESET_PIN            8
#define DATA_COMMAND_PIN     9
#define CHIP_SELECT_PIN      10


U8G2_ST7565_NHD_C12864_F_4W_HW_SPI u8g2(U8G2_R0, CHIP_SELECT_PIN, DATA_COMMAND_PIN, RESET_PIN);


void setup(void) {
  u8g2.begin();
  u8g2.setContrast(230);
  //u8g2.setPowerSave(1);
}

int counter = 0;
char str[10];

void loop(void) {
  sprintf(str, "%d", counter++);
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_fub42_tr);
    u8g2.drawStr(10,50,str);
  } while ( u8g2.nextPage() );
  powerDown(WAKEUP_DELAY_1_S);
}
