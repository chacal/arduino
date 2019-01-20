#pragma once
#include <openthread/thread.h>

#define ASSERT_OT(expr)               \
do {                                  \
  otError ot_err = expr;              \
  ASSERT(ot_err == OT_ERROR_NONE);    \
} while (false);

namespace thread {
  otInstance* initialize();
  void run();
}

