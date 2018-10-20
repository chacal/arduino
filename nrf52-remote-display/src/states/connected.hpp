#pragma once

#include <memory>
#include <variant>
#include <nrf_log.h>
#include <ble.h>
#include <ble_gap.h>
#include <peer_manager.h>
#include "display_command_handler.hpp"
#include "ble_data_service.hpp"
#include "cmd_protocol.hpp"
#include "status_bar.hpp"
#include "common.hpp"

#define RSSI_REPORT_CHANGE_THRESHOLD    3    //dBm
#define RSSI_REPORT_SKIP_COUNT         10

using namespace fsm;


namespace states {
  struct start;

  struct connected : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected");
      pm_conn_secure(context.conn_handle, false);  // Try to establish secure connection, this should invoke pairing on the remote side
      context.disp.on();
      sb = std::make_unique<status_bar>(context.disp);
      sd_ble_gap_rssi_start(context.conn_handle, RSSI_REPORT_CHANGE_THRESHOLD, RSSI_REPORT_SKIP_COUNT);
    }

    virtual void leave(Context &context) {
      NRF_LOG_INFO("Disconnected");
      sd_ble_gap_rssi_stop(context.conn_handle);
      context.conn_handle = BLE_CONN_HANDLE_INVALID;
      context.disp.off();
    }

    virtual void react(const ble_evt_t *p_ble_evt, Control &control, Context &context) {
      switch (p_ble_evt->header.evt_id) {
        case BLE_GAP_EVT_DISCONNECTED:
          control.changeTo<start>();
          break;

        case BLE_GAP_EVT_PASSKEY_DISPLAY:
          NRF_LOG_INFO("Passkey: %s", (uint32_t) p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
          break;

        case BLE_GAP_EVT_RSSI_CHANGED:
          auto rssi = p_ble_evt->evt.gap_evt.params.rssi_changed.rssi;
          sb->update_rssi(rssi);
          break;
      }
    }

    // This is called only after the connection has been secured as the RX characteristics has authorization that requires
    // secure connection
    virtual void react(const ble_data_service::rx_data &received_data, Control &control, Context &context) {
      NRF_LOG_DEBUG("Got %d bytes of data", received_data.len)
      auto cmd_seq = commands::parse(received_data);
      if (cmd_seq) {
        context.react(*cmd_seq);
      }
    }

    virtual void react(const commands::display_command_seq &commands, Control &control, Context &context) {
      NRF_LOG_DEBUG("Received %d commands", commands.size())
      for (auto &cmd : commands) {
        std::visit(display_command_handler{}, cmd);
      }
    }

    using Base::react;

  private:
    std::unique_ptr<status_bar> sb;
  };
}
