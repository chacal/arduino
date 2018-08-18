#include <ble_advdata.h>
#include <app_error.h>
#include <crc32.h>

#include "ble_sensor_advertising.h"

static ble_uuid_t               m_adv_uuids[] = {{DATA_SERVICE_SERVICE_UUID, DATA_SERVICE_UUID_TYPE}};
static ble_advdata_t            m_advdata;
static ble_advdata_manuf_data_t m_mf;

static void populate_adv_data(void *manuf_data, uint8_t manuf_data_len) {
  memset(&m_advdata, 0, sizeof(m_advdata));
  memset(&m_mf, 0, sizeof(m_mf));

  m_mf.company_identifier = 0xDADA;
  m_mf.data.p_data        = manuf_data;
  m_mf.data.size          = manuf_data_len;

  m_advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  m_advdata.flags                 = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  m_advdata.p_manuf_specific_data = &m_mf;
}

uint32_t ble_sensor_advertising_crc32(void *manuf_data, uint8_t manuf_data_len) {
  populate_adv_data(manuf_data, manuf_data_len);

  uint8_t encoded_adv_data[BLE_GAP_ADV_MAX_SIZE];
  memset(encoded_adv_data, 0, BLE_GAP_ADV_MAX_SIZE);
  uint16_t encoded_length = sizeof(encoded_adv_data);

  ble_advdata_encode(&m_advdata, encoded_adv_data, &encoded_length);

  return crc32_compute(encoded_adv_data, encoded_length, NULL);
}

void ble_sensor_advertising_init(void *manuf_data, uint8_t manuf_data_len) {
  populate_adv_data(manuf_data, manuf_data_len);

  ble_advdata_t scan_response_data;
  memset(&scan_response_data, 0, sizeof(scan_response_data));

  // Init scan response packet data
  scan_response_data.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  scan_response_data.uuids_complete.p_uuids  = m_adv_uuids;

  APP_ERROR_CHECK(ble_advdata_set(&m_advdata, &scan_response_data));
}


void ble_sensor_advertising_start(uint16_t adv_interval) {
  ble_gap_adv_params_t m_adv_params;
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = adv_interval;
  m_adv_params.timeout     = ADV_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CFG_TAG));
}

void ble_sensor_advertising_stop() {
  sd_ble_gap_adv_stop();
}
