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
#include "radio.h"

#define DEVICE_INSTANCE                      99
#define VCC_MEASUREMENT_INTERVAL_S            5
#define INA226_MEASUREMENT_INTERVAL_MS      500
#define SHUNT_RESISTANCE_OHMS           0.00025  // 0.25mΩ
#define MAX_EXPECTED_CURRENT_A              100

#pragma pack(1)

typedef struct {
  char tag;
  uint8_t instance;
  int16_t rawMeasurement;
  float shuntVoltageMilliVolts;
  float shuntCurrent;
  int vcc;
  unsigned long previousSampleTimeMicros;
} sensor_data_t;

sensor_data_t m_sensor_data = {
    .tag = 'c',
    .instance = DEVICE_INSTANCE,
    .previousSampleTimeMicros = 0
};

static void on_vcc_measurement(uint16_t vcc) {
  m_sensor_data.vcc = vcc;
  NRF_LOG_INFO("VCC: %d", vcc);
}

static void on_ina226_measurement(int16_t raw_measurement, double shunt_voltage_mV, double shunt_current) {
  NRF_LOG_INFO("Raw: %d  Shunt: " NRF_LOG_FLOAT_MARKER "mV", raw_measurement, NRF_LOG_FLOAT(shunt_voltage_mV));
  NRF_LOG_INFO("Current: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(shunt_current));

  m_sensor_data.rawMeasurement = raw_measurement;
  m_sensor_data.shuntVoltageMilliVolts = (float)shunt_voltage_mV;
  m_sensor_data.shuntCurrent = (float)shunt_current;

  radio_send((uint8_t *)&m_sensor_data, sizeof(m_sensor_data));
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

  NRF_LOG_INFO("Initializing clocks..");
  util_start_clocks();
  NRF_LOG_INFO("Initializing app_timer..");
  APP_ERROR_CHECK(app_timer_init());
  NRF_LOG_INFO("Initializing radio..");
  radio_init();
  NRF_LOG_INFO("Radio initialized!");

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  ina226_init(INA226_MEASUREMENT_INTERVAL_MS, SHUNT_RESISTANCE_OHMS, MAX_EXPECTED_CURRENT_A, on_ina226_measurement);


  for(;;) {
    power_manage();
  }
}
