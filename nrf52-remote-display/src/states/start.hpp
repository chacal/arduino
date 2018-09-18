#pragma once

#include <nrf_log.h>
#include "common.hpp"

using namespace fsm;

namespace states {
  struct idle;

  struct start : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Start");
    }

    void transition(Control &control, Context &context) {
      control.changeTo<idle>();
    }
  };
}
