#pragma once

#include <stdint.h>
#include "named_type.hpp"

namespace util {
  using seconds = named_type<uint32_t, struct seconds_param>;

  seconds calculate_seconds_left(uint32_t start_time_ticks, const seconds &duration_s);
}

