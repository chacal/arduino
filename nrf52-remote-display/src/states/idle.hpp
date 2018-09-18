#pragma once

#include <nrf_log.h>
#include <app_timer.h>
#include <state_machine.hpp>
#include "states_common.hpp"


APP_TIMER_DEF(adc_timer);

namespace states {
  struct start;

  struct idle : M::Base {
    struct timer_elapsed_evt {};

    idle() {
      NRF_LOG_INFO("Creating idle");
      app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, sample_vcc);
    }

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Enter idle");
      app_timer_start(adc_timer, APP_TIMER_TICKS(1000), &context);
    }

    void react(const timer_elapsed_evt &event, Control &control, Context &context) {
      NRF_LOG_INFO("Reacting in idle");
      control.changeTo<start>();
    }

    using M::Base::react;

    virtual void leave(Context &context) {
      NRF_LOG_INFO("Leave idle")
      app_timer_stop(adc_timer);
    }

    static void sample_vcc(void *ctx) {
      NRF_LOG_INFO("Timer!");
      auto c = static_cast<states::Context *>(ctx);
      c->react(timer_elapsed_evt{});
    }
  };
}

