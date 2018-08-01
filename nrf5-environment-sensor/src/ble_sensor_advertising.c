#include <ble_advdata.h>
#include <app_error.h>

#include "ble_sensor_advertising.h"
#include "ble_config.h"

static ble_uuid_t m_adv_uuids[] = {{DATA_SERVICE_SERVICE_UUID, DATA_SERVICE_UUID_TYPE}};


void ble_sensor_advertising_init(void *manuf_data, uint8_t manuf_data_len) {
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


void ble_sensor_advertising_start() {
  ble_gap_adv_params_t m_adv_params;
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = ADV_INTERVAL;
  m_adv_params.timeout     = ADV_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, APP_BLE_CFG_TAG));
}

void ble_sensor_advertising_stop() {
  sd_ble_gap_adv_stop();
}
