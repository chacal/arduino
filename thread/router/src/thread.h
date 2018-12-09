#pragma once

#define ASSERT_OT(expr)               \
do {                                  \
  otError ot_err = expr;              \
  ASSERT(ot_err == OT_ERROR_NONE);    \
} while (false);


void thread_initialize();