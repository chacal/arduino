#pragma once

#include <nrf_log.h>
#include <peer_manager.h>
#include "common.hpp"

using namespace fsm;

namespace states {
  struct idle;
  struct adv_with_whitelist;

  struct start : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Start");
    }

    void transition(Control &control, Context &context) {
      if(pm_peer_count() == 0) {
        control.changeTo<idle>();
      } else {
        control.changeTo<adv_with_whitelist>();
      }
    }
  };
}
