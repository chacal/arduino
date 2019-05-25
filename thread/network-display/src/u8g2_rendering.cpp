#include "u8g2_rendering.hpp"

const uint8_t *font_for_size(const font_size &size) {
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


void u8g2_rendering::draw_str(const point &bottom_left, const font_size &size, const std::string &str) {
  const uint8_t *font = font_for_size(size);
  u8g2_SetFont(&u8g2, font);
  u8g2_DrawStr(&u8g2, bottom_left.x, bottom_left.y, str.c_str());
}

void u8g2_rendering::draw_line(const point &start, const point &end) {
  u8g2_DrawLine(&u8g2, start.x, start.y, end.x, end.y);
}

void u8g2_rendering::draw_box(const point &upper_left, const width &w, const height &h) {
  u8g2_DrawBox(&u8g2, upper_left.x, upper_left.y, w.get(), h.get());
}

void u8g2_rendering::clear_area(const point &upper_left, const width &w, const height &h) {
  auto color = u8g2_GetDrawColor(&u8g2);
  u8g2_SetDrawColor(&u8g2, 0);
  draw_box(upper_left, w, h);
  u8g2_SetDrawColor(&u8g2, color);
}

void u8g2_rendering::draw_frame(const point &upper_left, const width &w, const height &h) {
  u8g2_DrawFrame(&u8g2, upper_left.x, upper_left.y, w.get(), h.get());
}

uint8_t u8g2_rendering::centered_x(const std::string &str, const font_size &size) {
  u8g2_SetFont(&u8g2, font_for_size(size));
  int x = u8g2.width / 2 - u8g2_GetStrWidth(&u8g2, str.c_str()) / 2;
  return x >= 0 ? x : 0;
}

