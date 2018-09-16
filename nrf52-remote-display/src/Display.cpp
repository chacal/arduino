#include "display.hpp"

#define CONTRAST       230
#define SCREEN_WIDTH   128

display::display() {
  u8g2_Setup_st7565_nhd_c12864_f(&u8g2, U8G2_R0, adapter.u8x8_byte_nrf52_hw_spi, adapter.u8x8_gpio_and_delay_nrf52);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetContrast(&u8g2, CONTRAST);
  u8g2_SetFont(&u8g2, u8g2_font_helvB10_tr);
  u8g2_ClearDisplay(&u8g2);
}

void display::on() {
  clear();
  render();
  u8g2_SetPowerSave(&u8g2, 0);
}

void display::off() {
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

void display::draw_str(uint8_t x, uint8_t y, uint8_t font_size, char *str) {
  const uint8_t *font = font_for_size(font_size);
  u8g2_SetFont(&u8g2, font);
  u8g2_DrawStr(&u8g2, x, y, str);
}

void display::clear() {
  u8g2_ClearBuffer(&u8g2);
}

void display::draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y) {
  u8g2_DrawLine(&u8g2, start_x, start_y, end_x, end_y);
}

void display::render() {
  u8g2_SendBuffer(&u8g2);
}

uint8_t display::centered_x(const char *str, uint8_t font_size) {
  u8g2_SetFont(&u8g2, font_for_size(font_size));
  int x = SCREEN_WIDTH / 2 - u8g2_GetStrWidth(&u8g2, str) / 2;
  return x >= 0 ? x : 0;
}
