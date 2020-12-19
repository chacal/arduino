#pragma once

#include <stdint.h>
#include <chrono>
#include <app_timer.h>
#include <string>
#include <openthread/netdata.h>

namespace util {
  using rtc_ticks = std::chrono::duration<uint32_t, std::ratio<1, APP_TIMER_CLOCK_FREQ>>;

  std::string get_state_json();

  void log_ipv6_address(const otNetifAddress &address);

  std::string ipv6_to_str(const otNetifAddress &address);

  std::string get_device_id();

  otNetifAddress create_ip6_addr_for_prefix(const otBorderRouterConfig &conf);

  bool has_addr_for_prefix(otInstance *instance, const otIp6Prefix &prefix);

  void print_addresses(otInstance *instance);
}
