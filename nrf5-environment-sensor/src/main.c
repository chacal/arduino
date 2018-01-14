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
#include "ble_support.h"

#define DEVICE_NAME                      "S300"
#define VCC_MEASUREMENT_INTERVAL_S          120
#define BME280_MEASUREMENT_INTERVAL_S        30

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

static void on_vcc_measurement(uint16_t vcc) {
  m_sensor_data.vcc = vcc;
  ble_support_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}

static void on_bme280_measurement(double temperature, double pressure, double humidity) {
  m_sensor_data.temperature = (int16_t) (temperature * 100);  // -327.68°C - +327.67°C
  m_sensor_data.pressure    = (uint16_t) (pressure * 10);     // 0 - 6553.5 mbar
  m_sensor_data.humidity    = (uint16_t) (humidity * 100);    // 0 - 655.35 %H
  ble_support_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
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

  ble_support_init(DEVICE_NAME);
  ble_support_advertising_init(&m_sensor_data, sizeof(m_sensor_data));

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  bme280_init(BME280_MEASUREMENT_INTERVAL_S * 1000, on_bme280_measurement);

  ble_support_advertising_start();

  NRF_LOG_INFO("BLE Beacon started");

  for(;;) {
    power_manage();
  }
}
