#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <string.h>
#include <ble_advdata.h>
#include "ble_support.h"

#define APP_BLE_CFG_TAG                 1
#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(1000, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */

#define TX_POWER_LEVEL                  4                                 /** Max power, +4dBm */


void ble_support_init() {
  APP_ERROR_CHECK(nrf_sdh_enable_request());

  uint32_t ram_start = 0;
  APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(APP_BLE_CFG_TAG, &ram_start));

  APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
  APP_ERROR_CHECK(sd_ble_gap_tx_power_set(TX_POWER_LEVEL));
}

void ble_support_advertising_init(const char *device_name, void *manuf_data, uint8_t manuf_data_len) {
  uint32_t err_code;

  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (uint8_t *)device_name, strlen(device_name));
  APP_ERROR_CHECK(err_code);


  ble_advdata_t advdata;

  ble_advdata_manuf_data_t mf;
  mf.company_identifier = 0xDADA;
  mf.data.p_data        = manuf_data;
  mf.data.size          = manuf_data_len;

  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  advdata.flags                 = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  advdata.p_manuf_specific_data = &mf;

  err_code = ble_advdata_set(&advdata, NULL);
  APP_ERROR_CHECK(err_code);
}


void ble_support_advertising_start() {
  ble_gap_adv_params_t m_adv_params;
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
  m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, BLE_CONN_CFG_TAG_DEFAULT));
}

