#include <stdint.h>
#include <nrf_log.h>
#include "ina226.h"
#include "ble_sensor_advertising.h"
#include <nrf_nvic.h>

#define SHUNT_RESISTANCE_OHMS                                 0.00025  // 0.25mΩ
#define MAX_EXPECTED_CURRENT_A                                    100
#define MEASUREMENT_INTERVAL_MS                                   200
#define CURRENT_ADV_INTERVAL_MS     MSEC_TO_UNITS(500, UNIT_0_625_MS)

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  int16_t  raw_measurement;
  float    shunt_current;
  uint16_t vcc;
  uint16_t msg_counter;
} current_sensor_data_t;

static current_sensor_data_t m_sensor_data = {
    .ttl         = 2,
    .tag         = 'n',
    .crc         = 0,
    .msg_counter = 0
};

static void update_advertisement_data() {
  m_sensor_data.crc = 0;
  m_sensor_data.crc = ble_sensor_advertising_crc32(&m_sensor_data, sizeof(m_sensor_data));  // Calculate CRC
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}

void on_ina226_measurement(int16_t raw_measurement, double shunt_voltage_mV, double shunt_current_A, double bus_voltage_mV) {
  NRF_LOG_INFO("Raw: %d  Shunt: " NRF_LOG_FLOAT_MARKER "mV", raw_measurement, NRF_LOG_FLOAT(shunt_voltage_mV));
  NRF_LOG_INFO("Current: " NRF_LOG_FLOAT_MARKER "A", NRF_LOG_FLOAT(shunt_current_A));
  NRF_LOG_INFO("Bus: " NRF_LOG_FLOAT_MARKER "V", NRF_LOG_FLOAT(bus_voltage_mV / 1000));

  m_sensor_data.raw_measurement = raw_measurement;
  m_sensor_data.shunt_current = (float)shunt_current_A;
  m_sensor_data.vcc = (uint16_t)bus_voltage_mV;
  m_sensor_data.msg_counter++;

  update_advertisement_data();
}


void current_sensor_start() {
  update_advertisement_data();
  ina226_init(MEASUREMENT_INTERVAL_MS, SHUNT_RESISTANCE_OHMS, MAX_EXPECTED_CURRENT_A, on_ina226_measurement);

  ble_sensor_advertising_start(CURRENT_ADV_INTERVAL_MS);

  NRF_LOG_INFO("BLE current sensor started");
}
