#include <nrf_log.h>
#include "status_bar.h"

#define RSSI_REPORT_CHANGE_THRESHOLD    2    //dBm
#define RSSI_REPORT_SKIP_COUNT          5

static uint16_t           m_conn_handle = BLE_CONN_HANDLE_INVALID;    /**< Handle of the current connection. */
static int8_t             m_rssi;
static ble_gap_conn_sec_t m_conn_sec;


static void update_conn_sec_status(uint16_t conn_handle) {
  if(sd_ble_gap_conn_sec_get(conn_handle, &m_conn_sec) == NRF_SUCCESS) {
    NRF_LOG_INFO("Conn security: Mode: %d, Level: %d, Key size: %d \n", m_conn_sec.sec_mode.sm, m_conn_sec.sec_mode.lv, m_conn_sec.encr_key_size);
  }
}

static void on_rssi_changed(int8_t rssi) {
  m_rssi = rssi;
  NRF_LOG_INFO("RSSI: %d\n", m_rssi);
}

void status_bar_on_ble_evt(ble_evt_t *p_ble_evt) {
  switch(p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_CONNECTED:
      m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
      sd_ble_gap_rssi_start(m_conn_handle, RSSI_REPORT_CHANGE_THRESHOLD, RSSI_REPORT_SKIP_COUNT);
      update_conn_sec_status(m_conn_handle);
      break;

    case BLE_GAP_EVT_DISCONNECTED:
      sd_ble_gap_rssi_stop(m_conn_handle);
      m_conn_handle = BLE_CONN_HANDLE_INVALID;
      break;

    case BLE_GAP_EVT_CONN_SEC_UPDATE:
      update_conn_sec_status(m_conn_handle);
      break;

    case BLE_GAP_EVT_RSSI_CHANGED:
      on_rssi_changed(p_ble_evt->evt.gap_evt.params.rssi_changed.rssi);
      break;

    default:
      break;
  }
}
