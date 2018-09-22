#pragma once

#include <nrf_log.h>
#include <app_timer.h>
#include "common.hpp"
#include "ble_support_adv.hpp"
#include "util.hpp"
#include "rendering.hpp"

#define DISCOVERABLE_TIMEOUT         util::seconds(60)
#define DISCOVERABLE_TIMER_PERIOD    APP_TIMER_TICKS(1000)

using namespace fsm;


namespace states {
  struct start;

  struct discoverable : Base {
    struct timer_elapsed {};

    discoverable() {
      app_timer_create(&discoverable_timer, APP_TIMER_MODE_REPEATED,
                       [](void *ctx) { static_cast<Context *>(ctx)->react(timer_elapsed{}); });
    }

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Discoverable");
      ble_support::adv::discoverable_start();
      app_timer_start(discoverable_timer, DISCOVERABLE_TIMER_PERIOD, &context);
      discoverable_start_time = app_timer_cnt_get();
    }

    virtual void leave(Context &context) {
      app_timer_stop(discoverable_timer);
    }

    virtual void react(const timer_elapsed &event, Control &control, Context &context) {
      auto discoverable_time_left = util::calculate_seconds_left(discoverable_start_time, DISCOVERABLE_TIMEOUT);

      if (discoverable_time_left.get() <= 0) {
        NRF_LOG_INFO("Discoverable timeout");
        control.changeTo<start>();
      } else {
        rendering::render_discoverable_state(context.disp, discoverable_time_left);
      }
    }


  private:
    app_timer_t          discoverable_timer_data = {{0}};
    const app_timer_id_t discoverable_timer      = &discoverable_timer_data;
    uint32_t             discoverable_start_time;
  };
}
