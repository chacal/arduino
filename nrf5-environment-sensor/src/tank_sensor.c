#include <stdint.h>
#include <app_timer.h>
#include <nrf_drv_saadc.h>
#include <math.h>
#include "ble_sensor_advertising.h"
#include "vcc_measurement.h"
#include "nrf_log.h"
#include "ntc_thermistor.h"
#include "adc_utils.h"

/*
 * Wiring:
 * - 0-180ohm tank gauge between VCC and DIVIDER_INPUT_PIN
 * - 1k resistor between DIVIDER_INPUT_PIN and DIVIDER_DRIVE_PIN
 */

#define TANK_MEASUREMENT_INTERVAL_S              15
#define TANK_FULL_RESISTANCE                    178

#define DIVIDER_INPUT_PIN      NRF_SAADC_INPUT_AIN0     // == Pin P0.02, tank level is measured from this pin
#define DIVIDER_DRIVE_PIN                         3     // This set to HIGH when measuring tank level
#define DIVIDER_GND_RESISTOR                   1000     // Resistor from DIVIDER_INPUT_PIN to GND
#define ADC_CHANNEL                               1     // ADC channel to use, must be different than VCC measurement channel


#pragma pack(1)

APP_TIMER_DEF(m_measurement_timer);

typedef struct {
  uint8_t  ttl;
  uint16_t tag;
  uint32_t crc;
  uint8_t  tank_level_percentage;
  uint16_t vcc;
} tank_sensor_data_t;

static tank_sensor_data_t m_sensor_data = {
    .ttl                   = 2,
    .tag                   = 'w',
    .crc                   = 0,
    .tank_level_percentage = 0,
    .vcc                   = 0
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

static void on_tank_level_measurement_timer(void *ctx) {
  double  R                     = adc_measure_divider_resistance();
  uint8_t tank_level_percentage = (uint8_t) fmin((100 * R / TANK_FULL_RESISTANCE), 100);  // Cap max tank value to 100%
  m_sensor_data.tank_level_percentage = tank_level_percentage;
  /*
  NRF_LOG_INFO("R: "
                   NRF_LOG_FLOAT_MARKER
                   " Tank level: %d", NRF_LOG_FLOAT(R), m_sensor_data.tank_level_percentage);
                   */
  update_advertisement_data();
}


void tank_sensor_start() {
  update_advertisement_data();

  vcc_measurement_init(VCC_MEASUREMENT_INTERVAL_S * 1000, on_vcc_measurement);

  adc_utils_init(ADC_CHANNEL, DIVIDER_INPUT_PIN, DIVIDER_DRIVE_PIN, DIVIDER_GND_RESISTOR, false, false);

  app_timer_create(&m_measurement_timer, APP_TIMER_MODE_REPEATED, on_tank_level_measurement_timer);
  app_timer_start(m_measurement_timer, APP_TIMER_TICKS(TANK_MEASUREMENT_INTERVAL_S * 1000), NULL);

  ble_sensor_advertising_start(DEFAULT_ADV_INTERVAL);

  NRF_LOG_INFO("BLE tank sensor started");
}
