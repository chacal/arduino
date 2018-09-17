#pragma once

#include <nrf_log.h>
#include "states_common.hpp"

namespace states {
  struct idle : M::Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Idle");
    }
  };
}
