#include "vcc_measurement.h"
#include <app_error.h>
#include <app_timer.h>
#include <nrf_drv_saadc.h>
#include "sdk_config.h"

#define EXT_VOLTAGE_MEAS_PIN    NRF_SAADC_INPUT_AIN4  // P0.28
#define VOLTAGE_DIVIDER_R1      1000     // Resistor between VIN and VIN_MEAS pin
#define VOLTAGE_DIVIDER_R2       910     // Resistor between VIN_MEAS and GND

static vcc_measurement_cb_t m_measurement_cb;
static vcc_measurement_cb_t m_ext_measurement_cb;


#define INT_ADC_CHANNEL       0   // ADC channel to use to measure internal VCC
#define EXT_ADC_CHANNEL       2   // ADC channel to use to measure external voltage

static bool m_adc_initialized = false;

static void sample_vcc(void *ctx) {
  nrf_saadc_value_t value;
  nrf_drv_saadc_sample_convert(INT_ADC_CHANNEL, &value);
  uint16_t vcc = (uint16_t) (value / (float) 4095 * 6 * 0.6 * 1000);  // ADC value (12-bit) * prescale (1/6) * 0.6V reference * mV
  m_measurement_cb(vcc);
}

static void sample_ext_voltage(void *ctx) {
  float multiplier = (VOLTAGE_DIVIDER_R1 + VOLTAGE_DIVIDER_R2) / (float) VOLTAGE_DIVIDER_R2;

  nrf_saadc_value_t value;
  nrf_drv_saadc_sample_convert(EXT_ADC_CHANNEL, &value);
  // ADC value (12-bit) * prescale (1/6) * 0.6V reference * mV
  uint16_t vcc = (uint16_t) (value / (float) 4095 * 6 * 0.6 * 1000 * multiplier);
  m_ext_measurement_cb(vcc);
}

static void on_adc_event(nrf_drv_saadc_evt_t const *event) {}

static void adc_init() {
  if (!m_adc_initialized) {
    nrf_drv_saadc_config_t config = NRF_DRV_SAADC_DEFAULT_CONFIG;
    APP_ERROR_CHECK(nrf_drv_saadc_init(&config, on_adc_event));
    m_adc_initialized = true;
  }
}

static void int_adc_init() {
  adc_init();

  nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);
  channel_config.burst = NRF_SAADC_BURST_ENABLED;
  APP_ERROR_CHECK(nrf_drv_saadc_channel_init(INT_ADC_CHANNEL, &channel_config));
}

static void ext_adc_init() {
  adc_init();

  nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(EXT_VOLTAGE_MEAS_PIN);
  channel_config.burst = NRF_SAADC_BURST_ENABLED;
  APP_ERROR_CHECK(nrf_drv_saadc_channel_init(EXT_ADC_CHANNEL, &channel_config));
}

void vcc_measurement_init(uint32_t measurement_interval_ms, vcc_measurement_cb_t callback) {
  m_measurement_cb = callback;
  int_adc_init();

  APP_TIMER_DEF(adc_timer);
  app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, sample_vcc);
  app_timer_start(adc_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);

  sample_vcc(NULL);
}

void ext_voltage_measurement_init(uint32_t measurement_interval_ms, vcc_measurement_cb_t callback) {
  m_ext_measurement_cb = callback;
  ext_adc_init();

  APP_TIMER_DEF(adc_timer);
  app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, sample_ext_voltage);
  app_timer_start(adc_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);

  sample_ext_voltage(NULL);
}
