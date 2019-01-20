#pragma once

#include <nrf_log.h>
#include <app_timer.h>
#include <mem_manager.h>

extern "C" {
#include <coap_dfu.h>
}

#include "common.hpp"

using namespace fsm;

namespace states {

  struct start : Base {
    virtual void enter(Context &context) {
      APP_ERROR_CHECK(nrf_mem_init());
      APP_ERROR_CHECK(app_timer_init());

      NRF_LOG_INFO("Start: Initializing OpenThread");
      otInstance *ot = thread::initialize();

      NRF_LOG_INFO("Start: Initializing DFU");
      APP_ERROR_CHECK(coap_dfu_init(ot));
    }
  };
}
