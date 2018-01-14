#include "vcc_measurement.h"
#include <app_error.h>
#include <app_timer.h>

#define APP_TIMER_PRESCALER 0

static vcc_measurement_cb_t m_measurement_cb;


/*
 * ------------------------------------------------------------------------------------------------
 * Implementation for NRF51 family
 * ------------------------------------------------------------------------------------------------
 */
#ifdef NRF51
#include <nrf_drv_adc.h>
#include <sdk_config_nrf51/sdk_config.h>

static nrf_drv_adc_channel_t m_adc_channel_config;

static void sample_vcc(void *ctx) {
  nrf_adc_value_t value;
  nrf_drv_adc_sample_convert(&m_adc_channel_config, &value);
  uint16_t vcc = (uint16_t) (value / (float) 1023 * 3 * 1.2 * 1000);  // ADC value * prescale (1/3) * 1.2V reference * mV
  m_measurement_cb(vcc);
}

static void adc_init() {
  nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
  APP_ERROR_CHECK(nrf_drv_adc_init(&config, NULL));

  m_adc_channel_config.config.config.input              = NRF_ADC_CONFIG_SCALING_SUPPLY_ONE_THIRD;
  m_adc_channel_config.config.config.external_reference = NRF_ADC_CONFIG_REF_VBG;
  m_adc_channel_config.config.config.resolution         = NRF_ADC_CONFIG_RES_10BIT;

  nrf_drv_adc_channel_enable(&m_adc_channel_config);
}


#else


/*
 * ------------------------------------------------------------------------------------------------
 * Implementation for NRF52 family
 * ------------------------------------------------------------------------------------------------
 */
#include <nrf_drv_saadc.h>
#include "sdk_config.h"

static uint8_t m_adc_channel = 0;

static void sample_vcc(void *ctx) {
  nrf_saadc_value_t value;
  nrf_drv_saadc_sample_convert(m_adc_channel, &value);
  uint16_t vcc = (uint16_t) (value / (float) 4095 * 6 * 0.6 * 1000);  // ADC value (12-bit) * prescale (1/6) * 0.6V reference * mV
  m_measurement_cb(vcc);
}

static void on_adc_event(nrf_drv_saadc_evt_t const *event) {}

static void adc_init() {
  nrf_drv_saadc_config_t config = NRF_DRV_SAADC_DEFAULT_CONFIG;
  APP_ERROR_CHECK(nrf_drv_saadc_init(&config, on_adc_event));

  nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);
  channel_config.burst = NRF_SAADC_BURST_ENABLED;
  APP_ERROR_CHECK(nrf_drv_saadc_channel_init(m_adc_channel, &channel_config));
}

#endif


/*
 * ------------------------------------------------------------------------------------------------
 * Common implementation
 * ------------------------------------------------------------------------------------------------
 */
void vcc_measurement_init(uint32_t measurement_interval_ms, vcc_measurement_cb_t callback) {
  m_measurement_cb = callback;
  adc_init();

  APP_TIMER_DEF(adc_timer);
  app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, sample_vcc);
  app_timer_start(adc_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);

  sample_vcc(NULL);
}
