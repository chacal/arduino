#pragma once

#include <stdint.h>
#include <vector>
#include <optional>

#include "radio.hpp"

namespace util {
  void start_clocks();
  std::string tohex(const uint8_t *in, size_t insz);
}

