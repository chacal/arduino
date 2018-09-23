#pragma once

#include <nrf_log.h>
#include <peer_manager.h>
#include <ble_gap.h>
#include "common.hpp"

using namespace fsm;

namespace states {

  struct base : Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Base");
    }

    virtual void react(const ble_evt_t *p_ble_evt, Control &control, Context &context) {
      switch (p_ble_evt->header.evt_id) {
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
          APP_ERROR_CHECK(sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle, NRF_SDH_BLE_GATT_MAX_MTU_SIZE));
          break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE: {
          auto p = &p_ble_evt->evt.gap_evt.params.conn_param_update.conn_params;
          NRF_LOG_INFO("Connection params:\nConnection interval: min: %dms max: %dms\nSlave latency: %d Supervision timeout: %dms",
                       p->min_conn_interval * 1.25, p->max_conn_interval * 1.25, p->slave_latency, p->conn_sup_timeout * 10)
          break;
        }

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST: {
          ble_gap_data_length_params_t dl_params = {0};  // Use auto settings
          APP_ERROR_CHECK(sd_ble_gap_data_length_update(p_ble_evt->evt.gap_evt.conn_handle, &dl_params, NULL));
          break;
        }

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE: {
          auto p = &p_ble_evt->evt.gap_evt.params.data_length_update.effective_params;
          NRF_LOG_INFO("Data length params: TX: %dB RX: %dB TX: %dus RX: %dus", p->max_tx_octets, p->max_rx_octets, p->max_tx_time_us,
                       p->max_rx_time_us)
          break;
        }

        case BLE_GAP_EVT_AUTH_STATUS: {
          auto p = &p_ble_evt->evt.gap_evt.params.auth_status;
          NRF_LOG_INFO("Auth status: %d Bonded: %d LESC: %d", p->auth_status, p->bonded, p->lesc);
          break;
        }

        case BLE_GAP_EVT_CONN_SEC_UPDATE: {
          ble_gap_conn_sec_t m_conn_sec;
          if(sd_ble_gap_conn_sec_get(p_ble_evt->evt.gap_evt.conn_handle, &m_conn_sec) == NRF_SUCCESS) {
            NRF_LOG_INFO("Conn security: Mode: %d, Level: %d, Key size: %d", m_conn_sec.sec_mode.sm, m_conn_sec.sec_mode.lv, m_conn_sec.encr_key_size);
          } else {
            NRF_LOG_INFO("Getting conn security failed.");
          }
        }
      }
    }

    using Base::react;
  };
}
