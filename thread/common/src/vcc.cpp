#include <nrfx_saadc.h>
#include <nrf_power.h>
#include "vcc.hpp"

static void on_adc_event(nrfx_saadc_evt_t const *event) {}

static uint8_t m_adc_channel = 0;

namespace vcc {
  void init() {
    nrfx_saadc_config_t config = NRFX_SAADC_DEFAULT_CONFIG;
    APP_ERROR_CHECK(nrfx_saadc_init(&config, on_adc_event));

    nrf_saadc_channel_config_t channel_config;

    if (nrf_power_mainregstatus_get() == NRF_POWER_MAINREGSTATUS_NORMAL) {
      channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_VDD);
      channel_config.gain = NRF_SAADC_GAIN1_6;
    } else {
      channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(SAADC_CH_PSELP_PSELP_VDDHDIV5);
      channel_config.gain = NRF_SAADC_GAIN1_2;
    }

    channel_config.burst = NRF_SAADC_BURST_ENABLED;
    APP_ERROR_CHECK(nrfx_saadc_channel_init(m_adc_channel, &channel_config));
  }

  uint16_t measure() {
    nrf_saadc_value_t value;
    nrfx_saadc_sample_convert(m_adc_channel, &value);

    if (nrf_power_mainregstatus_get() == NRF_POWER_MAINREGSTATUS_NORMAL) {
      // ADC value (12-bit) * gain (1/6) * 0.6V reference * mV
      auto vcc = (uint16_t) (value / (float) 4095 * 6 * 0.6 * 1000);
      return vcc;
    } else {
      // ADC value (12-bit) * gain (1/2) * vddh/5 * 0.6V reference * mV
      auto vcc = (uint16_t) (value / (float) 4095 * 2 * 5 * 0.6 * 1000);
      return vcc;
    }
  }
}