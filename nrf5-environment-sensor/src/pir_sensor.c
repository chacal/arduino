#include <stdint.h>
#include <nrf_log.h>
#include "pir_sensor.h"
#include "ble_sensor_advertising.h"
#include "vcc_measurement.h"
#include <nrf_nvic.h>
#include <nrf_drv_gpiote.h>
#include <ble_gap.h>
#include "random.h"

#define PIN_PIR_INPUT     11
#define PIN_PIR_POWER     12

#pragma pack(1)

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  uint8_t  motion_detected;
  uint16_t vcc;
  uint32_t message_id;
} pir_sensor_data_t;

static pir_sensor_data_t m_sensor_data = {
    .ttl             = 2,
    .tag             = 'k',
    .crc             = 0,
    .motion_detected = 0,
    .vcc             = 0,
    .message_id      = 0
};

static void update_advertisement_data() {
  m_sensor_data.message_id = random_get();
  m_sensor_data.crc = 0;
  m_sensor_data.crc = ble_sensor_advertising_crc32(&m_sensor_data, sizeof(m_sensor_data));  // Calculate CRC
  ble_sensor_advertising_init(&m_sensor_data, sizeof(m_sensor_data));   // Update advertising data
}

static void on_vcc_measurement(uint16_t vcc) {
  m_sensor_data.vcc = vcc;
  update_advertisement_data();
}

static void on_pir_change(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  m_sensor_data.motion_detected = (uint8_t) nrf_gpio_pin_read(PIN_PIR_INPUT);
  NRF_LOG_INFO("Motion detected: %d", m_sensor_data.motion_detected);

  ble_sensor_advertising_stop();
  update_advertisement_data();
  ble_sensor_advertising_start(DEFAULT_ADV_INTERVAL);
}

static void pir_input_init() {
  APP_ERROR_CHECK(nrf_drv_gpiote_init());
  nrf_drv_gpiote_in_config_t pir_pin_config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
  APP_ERROR_CHECK(nrf_drv_gpiote_in_init(PIN_PIR_INPUT, &pir_pin_config, on_pir_change));
  nrf_drv_gpiote_in_event_enable(PIN_PIR_INPUT, true);
}

static void pir_power_on_sensor() {
  nrf_drv_gpiote_out_config_t pir_power_pin_config = GPIOTE_CONFIG_OUT_SIMPLE(true);
  nrf_drv_gpiote_out_init(PIN_PIR_POWER, &pir_power_pin_config);
}

void pir_sensor_start() {
  random_init();
  update_advertisement_data();
  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);
  pir_input_init();
  pir_power_on_sensor();
  ble_sensor_advertising_start(DEFAULT_ADV_INTERVAL);

  NRF_LOG_INFO("BLE PIR sensor started");
}
