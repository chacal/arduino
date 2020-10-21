#pragma once

#include <functional>
#include <tuple>
#include <chrono>
#include <openthread/thread.h>

#define ASSERT_OT(expr)               \
do {                                  \
  otError ot_err = expr;              \
  ASSERT(ot_err == OT_ERROR_NONE);    \
} while (false);

using namespace std::chrono;

namespace thread {
  using thread_role_handler_t = std::function<void(otDeviceRole role)>;
  struct parent_info {
    uint16_t rloc16;
    uint8_t  link_quality_in;
    uint8_t  link_quality_out;
    int8_t   avg_rssi;
    int8_t   latest_rssi;
  };

  otInstance *initialize(const thread_role_handler_t &);

  void run();

  parent_info get_parent_info();
  void set_tx_power(int8_t power);
  int8_t get_tx_power();
  void set_normal_poll_period(milliseconds poll_period);
  milliseconds get_normal_poll_period();
  void set_increased_poll_period(milliseconds poll_period);
  milliseconds get_increased_poll_period();
  void set_increased_poll_duration(milliseconds duration);
  milliseconds get_increased_poll_duration();
  void begin_increased_poll_rate();
  void end_increased_poll_rate();
}

