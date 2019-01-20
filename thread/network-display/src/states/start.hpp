#pragma once

#include <nrf_log.h>
#include <app_timer.h>
#include <mem_manager.h>

extern "C" {
#include <coap_dfu.h>
}

#include "common.hpp"
#include "named_type.hpp"

using namespace fsm;

namespace states {
  struct connected;
  using thread_role = named_type<otDeviceRole, struct thread_role_param>;

  struct start : Base {
    virtual void enter(Context &context) {
      APP_ERROR_CHECK(nrf_mem_init());

      NRF_LOG_INFO("Start: Initializing OpenThread");
      auto       role_handler = [&](otDeviceRole role) { context.react(thread_role(role)); };
      otInstance *ot          = thread::initialize(role_handler);

      NRF_LOG_INFO("Start: Initializing DFU");
      APP_ERROR_CHECK(coap_dfu_init(ot));
    }

    virtual void react(const thread_role &role, Control &control, Context &context) {
      if (role.get() == OT_DEVICE_ROLE_CHILD) {
        NRF_LOG_INFO("Joined network as child.")
        control.changeTo<connected>();
      }
    }

    using Base::react;
  };
}
