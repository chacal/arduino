#pragma once

#include <nrf_log.h>
#include "common.hpp"

using namespace fsm;

namespace states {
  struct adv_with_whitelist : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Advertising with whitelist");
    }
  };
}
