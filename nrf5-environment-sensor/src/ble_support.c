#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <string.h>
#include <ble_advdata.h>
#include <nrf_log.h>
#include "ble_support.h"
#include "sdk_config.h"

#define APP_BLE_CFG_TAG                           1
#define ADV_TIMEOUT                               0
#define ADV_INTERVAL                              MSEC_TO_UNITS(5000, UNIT_0_625_MS)
#define MIN_CONN_INTERVAL                         MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL                         MSEC_TO_UNITS(500, UNIT_1_25_MS)
#define SLAVE_LATENCY                             2
#define CONN_SUP_TIMEOUT                          MSEC_TO_UNITS(10000, UNIT_10_MS)
#define TX_POWER_LEVEL                            4     // Max power, +4dBm

#define DATA_SERVICE_BASE_UUID                    {{0x7E, 0x64, 0x1F, 0x11, 0xF9, 0xC2, 0xA5, 0xBD, 0xE5, 0x4B, 0xDC, 0xC6, 0x00, 0x00, 0xCA, 0x61}}
#define DATA_SERVICE_UUID_TYPE                    BLE_UUID_TYPE_VENDOR_BEGIN
#define DATA_SERVICE_SERVICE_UUID                 0x0001     // Service UUID = 61ca0001c6dc4be5bda5c2f9111f647e
#define DATA_SERVICE_RX_CHARACTERISTIC_UUID       0x0002     // Char UUID    = 61ca0002c6dc4be5bda5c2f9111f647e
#define DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN    16


static ble_uuid_t               m_adv_uuids[] = {{DATA_SERVICE_SERVICE_UUID, DATA_SERVICE_UUID_TYPE}};
static uint16_t                 m_data_service_service_handle;
static ble_uuid_t               m_data_service_ble_uuid;
static ble_gatts_char_handles_t m_data_service_rx_char_handles;

static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context) {
  NRF_LOG_INFO("BLE event! %d", p_ble_evt->header.evt_id)

  switch(p_ble_evt->header.evt_id) {
    case BLE_GAP_EVT_DISCONNECTED:
      ble_support_advertising_start();
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

  // Register a handler for BLE events.
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

static void ble_service_init() {
  ret_code_t    err_code;
  ble_uuid128_t base_uuid = DATA_SERVICE_BASE_UUID;

  err_code = sd_ble_uuid_vs_add(&base_uuid, &m_data_service_ble_uuid.type);
  m_data_service_ble_uuid.uuid = DATA_SERVICE_SERVICE_UUID;
  APP_ERROR_CHECK(err_code);

  err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &m_data_service_ble_uuid, &m_data_service_service_handle);
  APP_ERROR_CHECK(err_code);
}


static void ble_characteristic_init() {
  ret_code_t err_code;

  ble_gatts_char_md_t char_md = {0};
  char_md.char_props.write         = true;
  char_md.char_props.write_wo_resp = true;
  char_md.char_props.read          = true;

  ble_uuid_t char_uuid;
  char_uuid.uuid = DATA_SERVICE_RX_CHARACTERISTIC_UUID;
  char_uuid.type = m_data_service_ble_uuid.type;

  ble_gatts_attr_md_t attr_md = {0};
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
  attr_md.vlen = 1;
  attr_md.vloc = BLE_GATTS_VLOC_STACK;

  ble_gatts_attr_t attr_char_value = {0};
  attr_char_value.p_uuid    = &char_uuid;
  attr_char_value.p_attr_md = &attr_md;
  attr_char_value.max_len   = DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN;


  err_code = sd_ble_gatts_characteristic_add(m_data_service_service_handle, &char_md, &attr_char_value, &m_data_service_rx_char_handles);
  APP_ERROR_CHECK(err_code);
}


void ble_support_init(const char *device_name) {
  softdevice_init();
  gap_params_init(device_name);
  ble_service_init();
  ble_characteristic_init();
}


void ble_support_advertising_init(void *manuf_data, uint8_t manuf_data_len) {
  ble_advdata_t advdata;
  memset(&advdata, 0, sizeof(advdata));

  ble_advdata_manuf_data_t mf;
  mf.company_identifier = 0xDADA;
  mf.data.p_data        = manuf_data;
  mf.data.size          = manuf_data_len;

  advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  advdata.flags                 = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  advdata.p_manuf_specific_data = &mf;


  ble_advdata_t scan_response_data;
  memset(&scan_response_data, 0, sizeof(scan_response_data));

  // Init scan response packet data
  scan_response_data.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  scan_response_data.uuids_complete.p_uuids  = m_adv_uuids;


  APP_ERROR_CHECK(ble_advdata_set(&advdata, &scan_response_data));
}


void ble_support_advertising_start() {
  ble_gap_adv_params_t m_adv_params;
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = ADV_INTERVAL;
  m_adv_params.timeout     = ADV_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CFG_TAG));
}

