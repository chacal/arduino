#pragma once

#include <stdint.h>
#include <chrono>
#include <app_timer.h>

namespace util {
  using rtc_ticks = std::chrono::duration<uint32_t , std::ratio<1, APP_TIMER_CLOCK_FREQ>>;
}
