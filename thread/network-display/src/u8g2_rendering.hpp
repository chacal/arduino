#pragma once

#include <string>
#include "named_type.hpp"

extern "C" {
#include <u8g2.h>
}

using font_size = named_type<uint8_t, struct font_size_param>;
using height = named_type<uint16_t , struct height_param>;
using width = named_type<uint16_t , struct width_param>;

struct point {
public:
  explicit point(uint16_t x, uint16_t y) : x{x}, y{y} {}

  uint16_t x = 0;
  uint16_t y = 0;
};


class u8g2_rendering {
public:
  void draw_str(const point &bottom_left, const font_size &, const std::string &);

  void draw_line(const point &start, const point &end);

  void draw_box(const point &upper_left, const width &, const height &);

  void draw_frame(const point &upper_left, const width &, const height &);

  void clear_area(const point &upper_left, const width &, const height &);

  uint8_t centered_x(const std::string &, const font_size &);


protected:
  u8g2_t u8g2{nullptr};
};
