#include "vcc_measurement.h"
#include <nrf_drv_adc.h>
#include <app_error.h>
#include <sdk_config_nrf51/sdk_config.h>
#include <app_timer.h>

#define APP_TIMER_PRESCALER 0

static nrf_drv_adc_channel_t m_adc_channel_config;
static vcc_measurement_cb_t m_measurement_cb;


static void sample_vcc(void *ctx) {
  nrf_adc_value_t value;
  nrf_drv_adc_sample_convert(&m_adc_channel_config, &value);
  uint16_t vcc = value / (float) 1023 * 3 * 1.2 * 1000;  // ADC value * prescale (1/3) * 1.2V reference * mV
  m_measurement_cb(vcc);
}

void vcc_measurement_init(uint32_t measurement_interval_ms, vcc_measurement_cb_t callback) {
  m_measurement_cb = callback;

  nrf_drv_adc_config_t config = NRF_DRV_ADC_DEFAULT_CONFIG;
  APP_ERROR_CHECK(nrf_drv_adc_init(&config, NULL));

  m_adc_channel_config.config.config.input              = NRF_ADC_CONFIG_SCALING_SUPPLY_ONE_THIRD;
  m_adc_channel_config.config.config.external_reference = NRF_ADC_CONFIG_REF_VBG;
  m_adc_channel_config.config.config.resolution         = NRF_ADC_CONFIG_RES_10BIT;

  nrf_drv_adc_channel_enable(&m_adc_channel_config);

  APP_TIMER_DEF(adc_timer);
  app_timer_create(&adc_timer, APP_TIMER_MODE_REPEATED, sample_vcc);
  app_timer_start(adc_timer, APP_TIMER_TICKS(measurement_interval_ms, APP_TIMER_PRESCALER), NULL);
}
