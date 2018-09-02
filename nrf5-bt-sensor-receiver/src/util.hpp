#pragma once

#include <stdint.h>
#include <vector>
#include <optional>

extern "C" {
#include "radio.h"
}

namespace Util {
  void start_clocks();
  std::string tohex(const uint8_t *in, size_t insz);
}

