#include <stdint.h>
#include "ble_sensor_advertising.h"
#include "vcc_measurement.h"
#include "nrf_log.h"
#include "ntc_thermistor.h"

#define NTC_MEASUREMENT_INTERVAL_S        11

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  int16_t  temperature;
  uint16_t vcc;
} temperature_sensor_data_t;

static temperature_sensor_data_t m_sensor_data = {
    .ttl         = 2,
    .tag         = 't',
    .crc         = 0,
    .temperature = 0,
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

static void on_ntc_thermistor_measurement(double temperature) {
  m_sensor_data.temperature = (int16_t) (temperature * 100);  // -327.68°C - +327.67°C
  update_advertisement_data();
}


void ntc_thermometer_sensor_start() {
  update_advertisement_data();

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  ntc_init(NTC_MEASUREMENT_INTERVAL_S * 1000, on_ntc_thermistor_measurement);

  ble_sensor_advertising_start(DEFAULT_ADV_INTERVAL);

  NRF_LOG_INFO("BLE NTC temperature sensor started");
}
