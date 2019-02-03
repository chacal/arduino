#pragma once

#include <nrf_log.h>
#include "periodic_timer.hpp"
#include "common.hpp"
#include "../coap_service.hpp"

#define COAP_TICK_PERIOD    std::chrono::seconds(1)

using namespace fsm;

namespace states {

  struct connected : Base {
    struct timer_ticked {};

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected");
      coap_tick_timer.start(&context);
      coap.initialize();
    }

    virtual void react(const timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    using Base::react;


  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(timer_ticked{}); }};
    coap_service coap;
  };
}

