#pragma once

#include <nrf_log.h>
#include "common.hpp"
#include "ble_support_adv.hpp"

using namespace fsm;

namespace states {
  struct discoverable : Base {

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Discoverable");
      ble_support::adv::discoverable_start();
    }
  };
}

