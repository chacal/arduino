#pragma once
#include <stdint.h>

namespace movingAvg {
  void init(uint16_t size);
  float calculate(int newValue);
}

