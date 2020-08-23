#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <app_error.h>
#include <ble_advdata.h>
#include <crc32.h>

#include "ble.hpp"

#define APP_BLE_CFG_TAG    1
#define TX_POWER_LEVEL     4     // Max power, +4dBm
#define ADV_INTERVAL       MSEC_TO_UNITS(500, UNIT_0_625_MS)

static uint8_t            m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t            m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
static ble_gap_adv_data_t m_adv_data   = {
  .adv_data = {
    .p_data = m_enc_advdata,
    .len    = BLE_GAP_ADV_SET_DATA_SIZE_MAX
  },
  .scan_rsp_data = {
    .p_data = NULL,
    .len    = 0
  }
};

static void encode_adv_data(uint8_t *const manuf_data, const uint8_t manuf_data_len, uint8_t *const p_encoded_data, uint16_t *const p_len) {
  ble_advdata_manuf_data_t mf = {};
  mf.company_identifier = 0xDABA;
  mf.data.p_data        = manuf_data;
  mf.data.size          = manuf_data_len;

  ble_advdata_t adv_data = {};
  adv_data.name_type             = BLE_ADVDATA_FULL_NAME;
  adv_data.flags                 = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  adv_data.p_manuf_specific_data = &mf;

  ble_advdata_encode(&adv_data, p_encoded_data, p_len);
}

uint32_t ble_sensor_advertising_crc32(uint8_t *const manuf_data, const uint8_t manuf_data_len) {
  uint8_t  encoded_adv_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX] = {};
  uint16_t encoded_length                                  = sizeof(encoded_adv_data);

  encode_adv_data(manuf_data, manuf_data_len, encoded_adv_data, &encoded_length);

  return crc32_compute(encoded_adv_data, encoded_length, nullptr);
}

void ble_sensor_advertising_init(uint8_t *const manuf_data, const uint8_t manuf_data_len) {
  encode_adv_data(manuf_data, manuf_data_len, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);

  ble_gap_adv_params_t m_adv_params = {};
  m_adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
  m_adv_params.p_peer_addr     = nullptr;    // Undirected advertisement.
  m_adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval        = ADV_INTERVAL;
  m_adv_params.duration        = 0;       // Never time out.

  APP_ERROR_CHECK(sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &m_adv_params));
  APP_ERROR_CHECK(sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, TX_POWER_LEVEL));
}


void ble_sensor_advertising_start() {
  APP_ERROR_CHECK(sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CFG_TAG));
}

void ble_sensor_advertising_stop() {
  APP_ERROR_CHECK(sd_ble_gap_adv_stop(m_adv_handle));
}


static void softdevice_init() {
  uint32_t ram_start = 0;
  APP_ERROR_CHECK(nrf_sdh_enable_request());
  APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(APP_BLE_CFG_TAG, &ram_start));
  APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
}


void ble_init(const char *device_name) {
  softdevice_init();

  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
  APP_ERROR_CHECK(sd_ble_gap_device_name_set(&sec_mode, reinterpret_cast<const uint8_t *>(device_name), strlen(device_name)));
}