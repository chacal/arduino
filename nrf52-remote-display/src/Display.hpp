#pragma once

#include <stdint.h>
#include "nrf_u8g2_adapter.hpp"
#include "named_type.hpp"

extern "C" {
#include <u8g2.h>
}

using font_size = named_type<uint8_t, struct font_size_param>;

class display {
public:
  display();

  void on();

  void off();

  void draw_str(uint8_t x, uint8_t y, font_size, char *str);

  void draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y);

  void render();

  void clear();

  uint8_t centered_x(const char *str, font_size);

private:
  u8g2_t u8g2;
  nrf_u8g2_adapter adapter;
};
