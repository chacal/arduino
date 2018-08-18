#include <stdint.h>
#include "ble_sensor_advertising.h"
#include "vcc_measurement.h"
#include "bme280.h"
#include "nrf_log.h"
#include "environmental_sensor.h"

#define BME280_MEASUREMENT_INTERVAL_S        30


#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  int16_t  temperature;
  uint16_t humidity;
  uint16_t pressure;
  uint16_t vcc;
} bme280_sensor_data_t;

static bme280_sensor_data_t m_sensor_data = {
    .ttl         = 2,
    .tag         = 'm',
    .crc         = 0,
    .temperature = 0,
    .humidity    = 0,
    .pressure    = 0,
    .vcc         = 0
};

static void update_advertisement_data() {
  m_sensor_data.crc = 0;
  m_sensor_data.crc = ble_sensor_advertising_crc32(&m_sensor_data, sizeof(m_sensor_data));  // Calculate CRC
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}

static void on_vcc_measurement(uint16_t vcc) {
  m_sensor_data.vcc = vcc;
  update_advertisement_data();
}

static void on_bme280_measurement(double temperature, double pressure, double humidity) {
  m_sensor_data.temperature = (int16_t) (temperature * 100);  // -327.68°C - +327.67°C
  m_sensor_data.pressure    = (uint16_t) (pressure * 10);     // 0 - 6553.5 mbar
  m_sensor_data.humidity    = (uint16_t) (humidity * 100);    // 0 - 655.35 %H
  update_advertisement_data();
}


void environmental_sensor_start() {
  update_advertisement_data();

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  bme280_init(BME280_MEASUREMENT_INTERVAL_S * 1000, on_bme280_measurement);

  ble_sensor_advertising_start(DEFAULT_ADV_INTERVAL);

  NRF_LOG_INFO("BLE environment sensor started");
}
