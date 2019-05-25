#pragma once

#include <cstdint>
#include <epd2in9/epd2in9.h>
#include <u8g2.h>

namespace EInkDisplay {
  void init();

  u8g2_t &getU8g2();

  void render();

  void sleep();
};

