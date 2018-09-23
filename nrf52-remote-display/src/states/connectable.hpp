#pragma once

#include <nrf_log.h>
#include <ble.h>
#include <ble_support_adv.hpp>
#include "common.hpp"

using namespace fsm;

namespace states {
  struct connected;

  struct connectable : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connectable");
      ble_support::adv::connectable_start();
    }

    virtual void leave(Context &context) {
      ble_support::adv::stop();
    }

    virtual void react(const ble_evt_t *p_ble_evt, Control &control, Context &context) {
      switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_CONNECTED:
          context.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
          control.changeTo<connected>();
          break;
      }
    }

    using Base::react;
  };
}
