#include "display.hpp"

#define CONTRAST       230

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

static const uint8_t *font_for_size(const font_size &size) {
  if (size.get() <= 8) {
    return u8g2_font_helvB08_tr;
  } else if (size.get() <= 10) {
    return u8g2_font_helvB10_tr;
  } else if (size.get() <= 12) {
    return u8g2_font_helvB12_tr;
  } else if (size.get() <= 14) {
    return u8g2_font_helvB14_tr;
  } else if (size.get() <= 18) {
    return u8g2_font_helvB18_tr;
  } else if (size.get() <= 24) {
    return u8g2_font_helvB24_tr;
  } else if (size.get() <= 25) {
    return u8g2_font_fub25_tn;
  } else if (size.get() <= 30) {
    return u8g2_font_fub30_tn;
  } else if (size.get() <= 35) {
    return u8g2_font_fub35_tn;
  } else {
    return u8g2_font_fub42_tn;
  }
}

void display::draw_str(const point &bottom_left, const font_size &size, const std::string &str) {
  const uint8_t *font = font_for_size(size);
  u8g2_SetFont(&u8g2, font);
  u8g2_DrawStr(&u8g2, bottom_left.x, bottom_left.y, str.c_str());
}

void display::clear() {
  u8g2_ClearBuffer(&u8g2);
}

void display::clear_area(const point &upper_left, const width &w, const height &h) {
  auto color = u8g2_GetDrawColor(&u8g2);
  u8g2_SetDrawColor(&u8g2, 0);
  draw_box(upper_left, w, h);
  u8g2_SetDrawColor(&u8g2, color);
}

void display::draw_line(const point &start, const point &end) {
  u8g2_DrawLine(&u8g2, start.x, start.y, end.x, end.y);
}

void display::draw_box(const point &upper_left, const width &w, const height &h) {
  u8g2_DrawBox(&u8g2, upper_left.x, upper_left.y, w.get(), h.get());
}

void display::draw_frame(const point &upper_left, const width &w, const height &h) {
  u8g2_DrawFrame(&u8g2, upper_left.x, upper_left.y, w.get(), h.get());
}

void display::render() {
  u8g2_SendBuffer(&u8g2);
}

uint8_t display::centered_x(const std::string &str, const font_size &size) {
  u8g2_SetFont(&u8g2, font_for_size(size));
  int x = SCREEN_WIDTH / 2 - u8g2_GetStrWidth(&u8g2, str.c_str()) / 2;
  return x >= 0 ? x : 0;
}
