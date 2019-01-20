#pragma once

#include <functional>
#include <openthread/thread.h>

#define ASSERT_OT(expr)               \
do {                                  \
  otError ot_err = expr;              \
  ASSERT(ot_err == OT_ERROR_NONE);    \
} while (false);

namespace thread {
  using thread_role_handler_t = std::function<void(otDeviceRole role)>;

  otInstance *initialize(const thread_role_handler_t&);

  void run();
}

