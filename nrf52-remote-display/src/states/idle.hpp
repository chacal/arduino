#pragma once

#include <nrf_log.h>
#include "common.hpp"

using namespace fsm;

namespace states {
  struct discoverable;

  struct idle : Base {

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Enter idle");
    }

    void transition(Control &control, Context &context) {
      control.changeTo<discoverable>();
    }
  };
}

