#include <stdint.h>
#include <nrf_log_default_backends.h>
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "vcc_measurement.h"
#include "util.h"
#include "ina226.h"
#include "sdk_config.h"

#define DEVICE_NAME                      "S214"
#define VCC_MEASUREMENT_INTERVAL_S            5
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
  NRF_LOG_INFO("VCC: %d", vcc);
}

static void on_bme280_measurement(double temperature, double pressure, double humidity) {
  m_sensor_data.temperature = (int16_t) (temperature * 100);  // -327.68°C - +327.67°C
  m_sensor_data.pressure    = (uint16_t) (pressure * 10);     // 0 - 6553.5 mbar
  m_sensor_data.humidity    = (uint16_t) (humidity * 100);    // 0 - 655.35 %H
}

static void power_manage(void) {
  // Always clear FPU IRQs to allow CPU to sleep. See: https://devzone.nordicsemi.com/question/87838/high-power-consumption-when-using-fpu/
  __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
  (void) __get_FPSCR();
  NVIC_ClearPendingIRQ(FPU_IRQn);
  __WFE();
  __WFI();
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("BLE environment sensor started");

  util_start_clocks();
  APP_ERROR_CHECK(app_timer_init());

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  //bme280_init(BME280_MEASUREMENT_INTERVAL_S * 1000, on_bme280_measurement);


  for(;;) {
    power_manage();
  }
}
