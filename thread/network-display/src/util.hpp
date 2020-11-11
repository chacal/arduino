#pragma once

#include <stdint.h>
#include <chrono>
#include <app_timer.h>
#include <string>

namespace util {
  using rtc_ticks = std::chrono::duration<uint32_t, std::ratio<1, APP_TIMER_CLOCK_FREQ>>;

  std::string get_status_json(std::string &instance);

  void log_ipv6_address(const uint8_t *address);

  std::string get_device_id();
}
