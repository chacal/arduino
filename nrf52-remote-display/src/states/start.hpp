#pragma once

#include <nrf_log.h>
#include "states_common.hpp"

namespace states {
  struct idle;

  struct start : M::Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Start");
    }

    void transition(Control &control, Context &context) {
      control.changeTo<idle>();
    }
  };
}
