#pragma once

#include <vector>
#include <variant>
#include <stdint.h>

namespace gzip {

  using result = std::variant<std::vector<uint8_t>, int>;

  result uncompress(const uint8_t *data, uint16_t len);
}