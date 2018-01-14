#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <string.h>
#include <nrf_log.h>
#include "ble_dfu_trigger_service.h"
#include "ble_config.h"
#include "ble_sensor_advertising.h"
#include "sdk_config.h"

static uint16_t                 m_service_handle;
static ble_uuid_t               m_ble_uuid;
static ble_gatts_char_handles_t m_rx_char_handles;


static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
  NRF_LOG_INFO("BLE event! %d", p_ble_evt->header.evt_id)

  switch(p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_DISCONNECTED:
      ble_sensor_advertising_start();
      break;
    case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST: {
      ble_gap_data_length_params_t dl_params = {0};  // Use auto settings
      APP_ERROR_CHECK(sd_ble_gap_data_length_update(p_ble_evt->evt.gap_evt.conn_handle, &dl_params, NULL));
      break;
    }
    case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
      APP_ERROR_CHECK(sd_ble_gatts_exchange_mtu_reply(p_ble_evt->evt.gatts_evt.conn_handle, BLE_GATT_ATT_MTU_DEFAULT));
      break;
  }
}


static void softdevice_init() {
  uint32_t ram_start = 0;

  APP_ERROR_CHECK(nrf_sdh_enable_request());
  APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(APP_BLE_CFG_TAG, &ram_start));
  APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
  APP_ERROR_CHECK(sd_ble_gap_tx_power_set(TX_POWER_LEVEL));

  NRF_SDH_BLE_OBSERVER(m_ble_observer, 2, ble_evt_handler, NULL);
}


static void gap_params_init(const char *device_name) {
  uint32_t                err_code;
  ble_gap_conn_params_t   gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (uint8_t *) device_name, strlen(device_name));
  APP_ERROR_CHECK(err_code);

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);
}


static void add_custom_ble_service() {
  ret_code_t    err_code;
  ble_uuid128_t base_uuid = DATA_SERVICE_BASE_UUID;

  err_code = sd_ble_uuid_vs_add(&base_uuid, &m_ble_uuid.type);
  m_ble_uuid.uuid = DATA_SERVICE_SERVICE_UUID;
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &m_ble_uuid, &m_service_handle);
  APP_ERROR_CHECK(err_code);
}


static void add_rx_characteristic() {
  ret_code_t err_code;

  ble_gatts_char_md_t char_md = {0};
  char_md.char_props.write         = true;
  char_md.char_props.write_wo_resp = true;
  char_md.char_props.read          = true;

  ble_uuid_t char_uuid;
  char_uuid.uuid = DATA_SERVICE_RX_CHARACTERISTIC_UUID;
  char_uuid.type = m_ble_uuid.type;

  ble_gatts_attr_md_t attr_md = {0};
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
  attr_md.vlen = 1;
  attr_md.vloc = BLE_GATTS_VLOC_STACK;

  ble_gatts_attr_t attr_char_value = {0};
  attr_char_value.p_uuid    = &char_uuid;
  attr_char_value.p_attr_md = &attr_md;
  attr_char_value.max_len   = DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN;


  err_code = sd_ble_gatts_characteristic_add(m_service_handle, &char_md, &attr_char_value, &m_rx_char_handles);
  APP_ERROR_CHECK(err_code);
}


void ble_dfu_trigger_service_init(const char *device_name) {
  softdevice_init();
  gap_params_init(device_name);
  add_custom_ble_service();
  add_rx_characteristic();
}


