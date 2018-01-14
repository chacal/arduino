#include <stdint.h>
#include <ble/common/ble_advdata.h>
#include <nrf_sdm.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>
#include <nrf_log_default_backends.h>
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "vcc_measurement.h"
#include "bme280.h"

#define APP_BLE_CFG_TAG                 1
#define APP_CFG_NON_CONN_ADV_TIMEOUT    0                                 /**< Time for which the device must be advertising in non-connectable mode (in seconds). 0 disables timeout. */
#define NON_CONNECTABLE_ADV_INTERVAL    MSEC_TO_UNITS(5000, UNIT_0_625_MS) /**< The advertising interval for non-connectable advertisement (100 ms). This value can vary between 100ms to 10.24s). */
#define VCC_MEASUREMENT_INTERVAL_S    120
#define BME280_MEASUREMENT_INTERVAL_S  30

#define DEVICE_NAME                     "S301"

#define TX_POWER_LEVEL                  4                                 /** Max power, +4dBm */

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  int16_t  temperature;
  uint16_t humidity;
  uint16_t pressure;
  uint16_t vcc;
} sensor_data_t;

static sensor_data_t m_sensor_data = {
    .ttl         = 2,
    .tag         = 'm',
    .temperature = 0,
    .humidity    = 0,
    .pressure    = 0,
    .vcc         = 0
};

static void advertising_init(void) {
  uint32_t err_code;

  ble_gap_conn_sec_mode_t sec_mode;
  BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&sec_mode);

  err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *) DEVICE_NAME, strlen(DEVICE_NAME));
  APP_ERROR_CHECK(err_code);


  ble_advdata_t advdata;

  ble_advdata_manuf_data_t manuf_data;
  manuf_data.company_identifier = 0xDADA;
  manuf_data.data.p_data        = (uint8_t *) &m_sensor_data;
  manuf_data.data.size          = sizeof(m_sensor_data);

  memset(&advdata, 0, sizeof(advdata));

  advdata.name_type             = BLE_ADVDATA_FULL_NAME;
  advdata.flags                 = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED;
  advdata.p_manuf_specific_data = &manuf_data;

  err_code = ble_advdata_set(&advdata, NULL);
  APP_ERROR_CHECK(err_code);
}


static void advertising_start(void) {
  ble_gap_adv_params_t m_adv_params;
  memset(&m_adv_params, 0, sizeof(m_adv_params));

  m_adv_params.type        = BLE_GAP_ADV_TYPE_ADV_NONCONN_IND;
  m_adv_params.p_peer_addr = NULL;                             // Undirected advertisement.
  m_adv_params.fp          = BLE_GAP_ADV_FP_ANY;
  m_adv_params.interval    = NON_CONNECTABLE_ADV_INTERVAL;
  m_adv_params.timeout     = APP_CFG_NON_CONN_ADV_TIMEOUT;
  APP_ERROR_CHECK(sd_ble_gap_adv_start(&m_adv_params, BLE_CONN_CFG_TAG_DEFAULT));
}

static void ble_stack_init(void) {
  APP_ERROR_CHECK(nrf_sdh_enable_request());

  uint32_t ram_start = 0;
  APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(APP_BLE_CFG_TAG, &ram_start));

  APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));
  APP_ERROR_CHECK(sd_ble_gap_tx_power_set(TX_POWER_LEVEL));
}

static void on_vcc_measurement(uint16_t vcc) {
  m_sensor_data.vcc = vcc;
  advertising_init();   // Update advertising data
}

static void on_bme280_measurement(double temperature, double pressure, double humidity) {
  m_sensor_data.temperature = (int16_t) (temperature * 100);  // -327.68°C - +327.67°C
  m_sensor_data.pressure    = (uint16_t) (pressure * 10);     // 0 - 6553.5 mbar
  m_sensor_data.humidity    = (uint16_t) (humidity * 100);    // 0 - 655.35 %H
  advertising_init();   // Update advertising data
}

static void power_manage(void) {
  // Always clear FPU IRQs to allow CPU to sleep. See: https://devzone.nordicsemi.com/question/87838/high-power-consumption-when-using-fpu/
  __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
  (void) __get_FPSCR();
  NVIC_ClearPendingIRQ(FPU_IRQn);
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  APP_ERROR_CHECK(app_timer_init());

  ble_stack_init();
  advertising_init();

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  bme280_init(BME280_MEASUREMENT_INTERVAL_S * 1000, on_bme280_measurement);

  NRF_LOG_INFO("BLE Beacon started");
  advertising_start();

  for(;;) {
    power_manage();
  }
}
