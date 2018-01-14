#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <string.h>
#include <ble_advdata.h>
#include <nrf_log.h>
#include <ble.h>
#include "ble_support.h"

#define APP_BLE_CFG_TAG                 1
#define ADV_TIMEOUT                     0
#define ADV_INTERVAL                    MSEC_TO_UNITS(5000, UNIT_0_625_MS)
#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)
#define SLAVE_LATENCY                   2
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(10000, UNIT_10_MS)

#define TX_POWER_LEVEL                  4                                 /** Max power, +4dBm */


static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context) {
  NRF_LOG_INFO("BLE event! %d", p_ble_evt->header.evt_id)
}


static void softdevice_init() {
  APP_ERROR_CHECK(nrf_sdh_enable_request());

  uint32_t ram_start = 0;
  APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(APP_BLE_CFG_TAG, &ram_start));

  APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
  APP_ERROR_CHECK(sd_ble_gap_tx_power_set(TX_POWER_LEVEL));

  // Register a handler for BLE events.
  NRF_SDH_BLE_OBSERVER(m_ble_observer, 2, ble_evt_handler, NULL);
}

static void gap_params_init(const char *device_name) {
  uint32_t err_code;
  ble_gap_conn_params_t   gap_conn_params;
  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (uint8_t *)device_name, strlen(device_name));
  APP_ERROR_CHECK(err_code);

  memset(&gap_conn_params, 0, sizeof(gap_conn_params));

  gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
  gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
  gap_conn_params.slave_latency     = SLAVE_LATENCY;
  gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

  err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
  APP_ERROR_CHECK(err_code);
}



void ble_support_init(const char *device_name) {
  softdevice_init();
  gap_params_init(device_name);
}


void ble_support_advertising_init(void *manuf_data, uint8_t manuf_data_len) {
  ble_advdata_t advdata;

  ble_advdata_manuf_data_t mf;
  mf.company_identifier = 0xDADA;
  mf.data.p_data        = manuf_data;
  mf.data.size          = manuf_data_len;

  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  advdata.flags                 = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  advdata.p_manuf_specific_data = &mf;

  APP_ERROR_CHECK(ble_advdata_set(&advdata, NULL));
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

