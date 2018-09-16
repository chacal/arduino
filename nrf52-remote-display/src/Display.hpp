#pragma once

#include <stdint.h>
#include "nrf_u8g2_adapter.hpp"

extern "C" {
#include <u8g2.h>
}

class display {
public:
  display();
  void on();
  void off();
  void draw_str(uint8_t x, uint8_t y, uint8_t font_size, char *str);
  void draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y);
  void render();
  void clear();
  uint8_t centered_x(const char *str, uint8_t font_size);

private:
  u8g2_t u8g2;
  nrf_u8g2_adapter adapter;
};
