#include <nrfx_saadc.h>
#include "vcc.hpp"

#define R1   3300     // kΩ
#define R2    910     // kΩ

static void on_adc_event(nrfx_saadc_evt_t const *event) {}

static uint8_t m_adc_channel = 0;

namespace vcc {
  void init() {
    nrfx_saadc_config_t config = NRFX_SAADC_DEFAULT_CONFIG;
    APP_ERROR_CHECK(nrfx_saadc_init(&config, on_adc_event));

    nrf_saadc_channel_config_t channel_config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN5);
    channel_config.gain  = NRF_SAADC_GAIN1_2;
    channel_config.burst = NRF_SAADC_BURST_ENABLED;
    APP_ERROR_CHECK(nrfx_saadc_channel_init(m_adc_channel, &channel_config));
  }

  uint16_t measure() {
    nrf_saadc_value_t value;
    nrfx_saadc_sample_convert(m_adc_channel, &value);

    float resistor_divider_gain = (R1 + R2) / (float) R2;

    // ADC value (12-bit) * prescale (1/2) * 0.6V reference * mV * PCB resistor divider
    auto vcc = (uint16_t) (value / (float) 4095 * 2 * 0.6 * 1000 * resistor_divider_gain);
    return vcc;
  }
}