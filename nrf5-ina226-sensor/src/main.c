#include <stdint.h>
#include <nrf_log_default_backends.h>
#include "bsp.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include <ble_dfu.h>
#include "vcc_measurement.h"
#include "ina226.h"
#include "ble_sensor_advertising.h"
#include "ble_dfu_trigger_service.h"

#define DEVICE_NAME                      "S300"
#define VCC_MEASUREMENT_INTERVAL_S            5
#define INA226_MEASUREMENT_INTERVAL_MS      500
#define SHUNT_RESISTANCE_OHMS           0.00025  // 0.25mΩ
#define MAX_EXPECTED_CURRENT_A              100

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  int16_t  raw_measurement;
  float shunt_voltage_milli_volts;
  float shunt_current;
  uint16_t vcc;
} sensor_data_t;

static sensor_data_t m_sensor_data = {
    .ttl         = 2,
    .tag         = 'n',
};


static void on_vcc_measurement(uint16_t vcc) {
  NRF_LOG_INFO("VCC: %d", vcc);
  m_sensor_data.vcc = vcc;
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}

static void on_ina226_measurement(int16_t raw_measurement, double shunt_voltage_mV, double shunt_current) {
  NRF_LOG_INFO("Raw: %d  Shunt: " NRF_LOG_FLOAT_MARKER "mV", raw_measurement, NRF_LOG_FLOAT(shunt_voltage_mV));
  NRF_LOG_INFO("Current: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(shunt_current));

  m_sensor_data.raw_measurement = raw_measurement;
  m_sensor_data.shunt_voltage_milli_volts = (float)shunt_voltage_mV;
  m_sensor_data.shunt_current = (float)shunt_current;

  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}

static void on_dfu_triggered() {
  NRF_LOG_INFO("Triggering DFU!")
  APP_ERROR_CHECK(sd_power_gpregret_clr(0, 0xffffffff));
  APP_ERROR_CHECK(sd_power_gpregret_set(0, BOOTLOADER_DFU_START));
  NVIC_SystemReset();
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

  ble_dfu_trigger_service_init(DEVICE_NAME, on_dfu_triggered);
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  ina226_init(INA226_MEASUREMENT_INTERVAL_MS, SHUNT_RESISTANCE_OHMS, MAX_EXPECTED_CURRENT_A, on_ina226_measurement);

  ble_sensor_advertising_start();

  NRF_LOG_INFO("BLE current sensor started");

  for(;;) {
    power_manage();
  }
}
