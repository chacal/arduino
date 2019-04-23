#include <nrf_gpio.h>
#include <nrf_drv_saadc.h>

static uint8_t  m_adc_channel;
static uint8_t  m_divider_drive_pin;
static uint32_t m_divider_gnd_resistance;
static bool     m_drive_pin_is_source;

static void adc_channel_init(uint8_t input_pin, bool use_internal_pulldown) {
  nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(input_pin);
  channel_config.reference  = NRF_SAADC_REFERENCE_VDD4,
  channel_config.gain       = NRF_SAADC_GAIN1_4,
  channel_config.burst      = NRF_SAADC_BURST_ENABLED;
  channel_config.acq_time   = NRF_SAADC_ACQTIME_15US;  // Long enough for max 200k resistance in source. See PS, page 364
  channel_config.resistor_p = use_internal_pulldown ? NRF_SAADC_RESISTOR_PULLDOWN : NRF_SAADC_RESISTOR_DISABLED;
  APP_ERROR_CHECK(nrf_drv_saadc_channel_init(m_adc_channel, &channel_config));
}

static void adc_divider_power_on() {
  nrf_gpio_cfg(
      m_divider_drive_pin,
      NRF_GPIO_PIN_DIR_OUTPUT,
      NRF_GPIO_PIN_INPUT_DISCONNECT,
      NRF_GPIO_PIN_NOPULL,
      NRF_GPIO_PIN_H0H1,
      NRF_GPIO_PIN_NOSENSE);
  if (m_drive_pin_is_source) {
    nrf_gpio_pin_set(m_divider_drive_pin);
  } else {
    nrf_gpio_pin_clear(m_divider_drive_pin);
  }
}

static void adc_divider_power_off() {
  nrf_gpio_cfg_default(m_divider_drive_pin);
}

void adc_utils_init(uint8_t adc_channel, uint8_t divider_input_pin, uint8_t divider_drive_pin, uint32_t divider_gnd_resistance,
                    bool use_internal_pulldown, bool drive_pin_is_source) {
  m_adc_channel            = adc_channel;
  m_divider_drive_pin      = divider_drive_pin;
  m_divider_gnd_resistance = divider_gnd_resistance;
  m_drive_pin_is_source    = drive_pin_is_source;

  adc_channel_init(divider_input_pin, use_internal_pulldown);

  adc_divider_power_off();
}

double adc_measure_divider_resistance() {
  nrf_saadc_value_t value;

  adc_divider_power_on();
  nrf_drv_saadc_sample_convert(m_adc_channel, &value);
  adc_divider_power_off();

  return m_divider_gnd_resistance * (4096.0f / value - 1);
}
