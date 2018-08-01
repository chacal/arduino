#include <stdint.h>
#include <nrf_log.h>
#include "pir_sensor.h"
#include "ble_sensor_advertising.h"
#include "vcc_measurement.h"
#include <nrf_nvic.h>

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint8_t  motion_detected;
  uint16_t vcc;
} pir_sensor_data_t;

static pir_sensor_data_t m_sensor_data = {
    .ttl             = 2,
    .tag             = 'k',
    .motion_detected = 0,
    .vcc             = 0
};

static void on_vcc_measurement(uint16_t vcc) {
  m_sensor_data.vcc = vcc;
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}


void pir_sensor_start() {
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);

  ble_sensor_advertising_start();

  NRF_LOG_INFO("BLE PIR sensor started");
}
