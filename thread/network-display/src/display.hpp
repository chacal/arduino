#pragma once

#include <vector>

class display {
public:
  virtual void on() = 0;

  virtual void render() = 0;

  virtual void clear() = 0;

  virtual void draw_fullscreen_bitmap(const std::vector<uint8_t> &) = 0;
};
