#pragma once

#include <nrf_log.h>
#include "common.hpp"

using namespace fsm;

namespace states {

  struct connected : Base {

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected");
    }
  };
}

