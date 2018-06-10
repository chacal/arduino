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
#define INA226_MEASUREMENT_INTERVAL_MS      500
#define SHUNT_RESISTANCE_OHMS           0.00025  // 0.25mΩ
#define MAX_EXPECTED_CURRENT_A              100

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

static void on_ina226_measurement(int16_t raw_measurement, double shunt_voltage_mV, double shunt_current) {
  NRF_LOG_INFO("Raw: %d  Shunt: " NRF_LOG_FLOAT_MARKER "mV", raw_measurement, NRF_LOG_FLOAT(shunt_voltage_mV));
  NRF_LOG_INFO("Current: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(shunt_current));
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
  ina226_init(INA226_MEASUREMENT_INTERVAL_MS, SHUNT_RESISTANCE_OHMS, MAX_EXPECTED_CURRENT_A, on_ina226_measurement);


  for(;;) {
    power_manage();
  }
}
