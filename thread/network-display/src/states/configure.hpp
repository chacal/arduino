#pragma once

#include <nrf_log.h>
#include <hfsm/machine_single.hpp>
#include "common.hpp"
#include "connected.hpp"

using namespace fsm;

namespace states {

  struct configure : Base {

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Configuring..");
    }

    void transition(Control &control, Context &context) {
      control.changeTo<connected>();
    }

    using Base::react;
  };
}
