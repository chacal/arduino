#include "Display.hpp"
#include "NRFu8g2Adapter.hpp"

#define CONTRAST       230
#define SCREEN_WIDTH   128

Display &Display::getInstance() {
  static Display theInstance;
  return theInstance;
}

Display::Display() {
  NRFu8g2Adapter adapter = NRFu8g2Adapter::getInstance();
  u8g2_Setup_st7565_nhd_c12864_f(&u8g2, U8G2_R0, adapter.u8x8_byte_nrf52_hw_spi, adapter.u8x8_gpio_and_delay_nrf52);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetContrast(&u8g2, CONTRAST);
  u8g2_SetFont(&u8g2, u8g2_font_helvB10_tr);
  u8g2_ClearDisplay(&u8g2);
}

void Display::on() {
  clear();
  render();
  u8g2_SetPowerSave(&u8g2, 0);
}

void Display::off() {
  u8g2_SetPowerSave(&u8g2, 1);
}

static const uint8_t *font_for_size(uint8_t font_size) {
  if(font_size <= 8) {
    return u8g2_font_helvB08_tr;
  } else if(font_size <= 10) {
    return u8g2_font_helvB10_tr;
  } else if(font_size <= 12) {
    return u8g2_font_helvB12_tr;
  } else if(font_size <= 14) {
    return u8g2_font_helvB14_tr;
  } else if(font_size <= 18) {
    return u8g2_font_helvB18_tr;
  } else if(font_size <= 24) {
    return u8g2_font_helvB24_tr;
  } else if(font_size <= 25) {
    return u8g2_font_fub25_tn;
  } else if(font_size <= 30) {
    return u8g2_font_fub30_tn;
  } else if(font_size <= 35) {
    return u8g2_font_fub35_tn;
  } else {
    return u8g2_font_fub42_tn;
  }
}

void Display::drawStr(uint8_t x, uint8_t y, uint8_t fontSize, char *str) {
  const uint8_t *font = font_for_size(fontSize);
  u8g2_SetFont(&u8g2, font);
  u8g2_DrawStr(&u8g2, x, y, str);
}

void Display::clear() {
  u8g2_ClearBuffer(&u8g2);
}

void Display::drawLine(uint8_t startX, uint8_t startY, uint8_t endX, uint8_t endY) {
  u8g2_DrawLine(&u8g2, startX, startY, endX, endY);
}

void Display::render() {
  u8g2_SendBuffer(&u8g2);
}

uint8_t Display::centeredX(const char *str, uint8_t fontSize) {
  u8g2_SetFont(&u8g2, font_for_size(fontSize));
  int x = SCREEN_WIDTH / 2 - u8g2_GetStrWidth(&u8g2, str) / 2;
  return x >= 0 ? x : 0;
}
