#pragma once

#include <nrf_log.h>
#include "common.hpp"
#include "ble_support_adv.hpp"
#include "util.hpp"
#include "rendering.hpp"
#include "countdown_timer.hpp"

#define DISCOVERABLE_TIMEOUT         std::chrono::seconds(60)
#define DISCOVERABLE_TIMER_PERIOD    std::chrono::seconds(1)

using namespace fsm;


namespace states {
  struct start;

  struct discoverable : Base {
    struct timer_elapsed {};

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Discoverable");
      ble_support::adv::discoverable_start();
      discoverable_timer.start(&context);
    }

    virtual void leave(Context &context) {
      discoverable_timer.stop();
    }

    virtual void react(const timer_elapsed &event, Control &control, Context &context) {
      auto discoverable_time_left = discoverable_timer.seconds_left();

      if (discoverable_time_left.count() <= 0) {
        NRF_LOG_INFO("Discoverable timeout");
        control.changeTo<start>();
      } else {
        rendering::render_discoverable_state(context.disp, discoverable_time_left);
      }
    }


  private:
    countdown_timer discoverable_timer{
        DISCOVERABLE_TIMEOUT,
        DISCOVERABLE_TIMER_PERIOD,
        [](void *ctx) { static_cast<Context *>(ctx)->react(timer_elapsed{}); }};
  };
}
