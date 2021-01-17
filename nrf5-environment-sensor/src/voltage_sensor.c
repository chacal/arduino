#include "nrf_log.h"

#include "voltage_sensor.h"
#include "ble_sensor_advertising.h"
#include "vcc_measurement.h"

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  uint16_t vcc;
} voltage_sensor_data_t;

static voltage_sensor_data_t m_sensor_data = {
  .ttl         = 2,
  .tag         = 'v',
  .crc         = 0,
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

void voltage_sensor_start() {
  update_advertisement_data();
  ext_voltage_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  ble_sensor_advertising_start(DEFAULT_ADV_INTERVAL);
  NRF_LOG_INFO("BLE voltage sensor started");
}
