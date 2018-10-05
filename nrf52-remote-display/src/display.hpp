#pragma once

#include <stdint.h>
#include <string>
#include "nrf_u8g2_adapter.hpp"
#include "named_type.hpp"

extern "C" {
#include <u8g2.h>
}

using font_size = named_type<uint8_t, struct font_size_param>;

struct point {
public:
  explicit point(uint8_t x, uint8_t y) : x{x}, y{y} {}

  uint8_t x = 0;
  uint8_t y = 0;
};


class display {
public:
  display();

  void on();

  void off();

  void draw_str(const point &bottom_left, const font_size &, const std::string &);

  void draw_line(const point &start, const point &end);

  void render();

  void clear();

  uint8_t centered_x(const std::string &, const font_size &);

private:
  u8g2_t u8g2;
  nrf_u8g2_adapter adapter;
};
