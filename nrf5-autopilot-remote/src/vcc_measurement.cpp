#include <app_error.h>
#include <nrfx_saadc.h>
#include <nrf_log.h>
#include "vcc_measurement.hpp"

#define ADC_CHANNEL       0   // ADC channel to use
#define VIN_MEAS_PIN      NRF_SAADC_INPUT_AIN2
#define VIN_MEAS_R1       2000  // 2000kOhm
#define VIN_MEAS_R2       2000  // 2000kOhm

static vcc_measurement_cb_t m_measurement_cb;
static nrf_saadc_value_t    sample_buf[1];

static void on_adc_event(nrfx_saadc_evt_t const *event) {
  if (event->type == NRFX_SAADC_EVT_DONE) {
    auto value = (VIN_MEAS_R1 + VIN_MEAS_R2) * event->data.done.p_buffer[0] / VIN_MEAS_R2;
    auto vcc   = (uint16_t) (value / (float) 4095 * 6 * 0.6 * 1000);  // ADC value (12-bit) * prescale (1/6) * 0.6V reference * mV
    m_measurement_cb(vcc);
  }
}

static void adc_init() {
  nrfx_saadc_config_t config = NRFX_SAADC_DEFAULT_CONFIG;
  APP_ERROR_CHECK(nrfx_saadc_init(&config, on_adc_event));

  nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(VIN_MEAS_PIN);
  channel_config.burst = NRF_SAADC_BURST_ENABLED;
  APP_ERROR_CHECK(nrfx_saadc_channel_init(ADC_CHANNEL, &channel_config));
}

void sample_vcc() {
  APP_ERROR_CHECK(nrfx_saadc_buffer_convert(sample_buf, 1));
  APP_ERROR_CHECK(nrfx_saadc_sample());
}

void vcc_measurement_init(vcc_measurement_cb_t callback) {
  m_measurement_cb = callback;
  adc_init();
  sample_vcc();
}
