#pragma once

#include <nrf_log.h>
#include <ble.h>
#include <ble_gap.h>
#include <ble_data_service.hpp>
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

        case BLE_GAP_EVT_PASSKEY_DISPLAY:
          NRF_LOG_INFO("Passkey: %s", (uint32_t) p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
          break;
      }
    }

    virtual void react(ble_data_service::rx_data &received_data, Control &control, Context &context) {
      NRF_LOG_INFO("Got %d bytes of data:", received_data.size())
      NRF_LOG_HEXDUMP_INFO(received_data.data(), received_data.size())
    }

    using Base::react;
  };
}
