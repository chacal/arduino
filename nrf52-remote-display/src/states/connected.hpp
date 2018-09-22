#pragma once

#include <nrf_log.h>
#include <ble.h>
#include <ble_gap.h>
#include "common.hpp"

using namespace fsm;


namespace states {
  struct start;

  struct connected : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected");
    }

    virtual void leave(Context &context) {
      NRF_LOG_INFO("Disconnected");
      context.conn_handle = BLE_CONN_HANDLE_INVALID;
    }

    virtual void react(const ble_evt_t *p_ble_evt, Control &control, Context &context) {
      switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
          control.changeTo<start>();
          break;
      }
    }

    using Base::react;
  };
}
