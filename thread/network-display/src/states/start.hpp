#pragma once

#include <nrf_log.h>
#include <app_timer.h>
#include <mem_manager.h>
#include <periodic_timer.hpp>

extern "C" {
#include <coap_dfu.h>
}

#include "common.hpp"
#include "named_type.hpp"
#include "power_manager.hpp"
#include "vcc.hpp"

#define COAP_TICK_PERIOD       std::chrono::seconds(1)

using namespace fsm;

namespace states {
  struct discover;
  using thread_role = named_type<otDeviceRole, struct thread_role_param>;

  struct start : Base {
    struct coap_timer_ticked {
    };

    virtual void enter(Context &context) {
      APP_ERROR_CHECK(nrf_mem_init());
      power_manager::init();
      vcc::init();

      NRF_LOG_INFO("Start: Initializing OpenThread");
      auto       role_handler = [&](otDeviceRole role) { context.react(thread_role(role)); };
      otInstance *ot          = thread::initialize(role_handler);

      NRF_LOG_INFO("Start: Initializing DFU");
      APP_ERROR_CHECK(coap_dfu_init(ot));
      coap_tick_timer.start(&context);
    }

    virtual void react(const thread_role &role, Control &control, Context &context) {
      if (role.get() == OT_DEVICE_ROLE_CHILD) {
        NRF_LOG_INFO("Joined network as child.")
        control.changeTo<discover>();
      }
    }

    virtual void react(const coap_timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    using Base::react;

  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(coap_timer_ticked{}); }};
  };
}
