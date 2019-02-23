#pragma once

#include <functional>
#include <tuple>
#include <openthread/thread.h>

#define ASSERT_OT(expr)               \
do {                                  \
  otError ot_err = expr;              \
  ASSERT(ot_err == OT_ERROR_NONE);    \
} while (false);

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
}

