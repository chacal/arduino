#include <app_timer.h>
#include <nrf_drv_gpiote.h>
#include <nrf_log.h>
#include <nrf_drv_saadc.h>
#include <math.h>

#include "ntc_thermistor.h"

/*
 * Wiring:
 * - 100k NTC thermistor between PIN_DIVIDER_VCC and PIN_DIVIDER_MEAS
 * - (internal 160k resistor is used between PIN_DIVIDER_MEAS and GND to create voltage divider)
 */

#define PIN_DIVIDER_MEAS            SAADC_CH_PSELP_PSELP_AnalogInput0   // == Pin P0.02, Temperature is measured from this pin
#define PIN_DIVIDER_VCC                3                                // This set to HIGH when measuring temperature

#define THERMISTOR_RESISTANCE   100000.0                                // NTC thermistor nominal value
#define THERMISTOR_BETA_COEF        3950                                // From thermistor's datasheet
#define THERMISTOR_NOMINAL_TEMP       25                                // From thermistor's datasheet (25°C)
#define DIVIDER_GND_RESISTOR    160000.0                                // nRF's internal resistor ladder's resistance, 160k. See PS, page 390
#define ADC_CHANNEL                    1                                // ADC channel to use, must be different than VCC measurement channel

APP_TIMER_DEF(m_measurement_timer);
static ntc_measurement_cb_t m_measurement_cb;

double calculate_temp(double ntc_resistance) {
  double steinhart;
  double R = ntc_resistance;
  steinhart = R / THERMISTOR_RESISTANCE;                  // (R/Ro)
  steinhart = log(steinhart);                             // ln(R/Ro)
  steinhart /= THERMISTOR_BETA_COEF;                      // 1/B * ln(R/Ro)
  steinhart += 1.0 / (THERMISTOR_NOMINAL_TEMP + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                            // Invert
  steinhart -= 273.15;                                    // convert to C
  return steinhart;
}

static double sample_ntc_resistance() {
  nrf_saadc_value_t value;
  nrf_drv_saadc_sample_convert(ADC_CHANNEL, &value);
  return DIVIDER_GND_RESISTOR * (4096.0f / value - 1);
}

static void adc_channel_init() {
  nrf_saadc_channel_config_t channel_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(PIN_DIVIDER_MEAS);
  channel_config.reference  = SAADC_CH_CONFIG_REFSEL_VDD1_4,
  channel_config.gain       = NRF_SAADC_GAIN1_4,
  channel_config.burst      = NRF_SAADC_BURST_ENABLED;
  channel_config.acq_time   = NRF_SAADC_ACQTIME_15US;  // Long enough for max 200k resistance in source. See PS, page 364
  channel_config.resistor_p = SAADC_CH_CONFIG_RESP_Pulldown;
  APP_ERROR_CHECK(nrf_drv_saadc_channel_init(ADC_CHANNEL, &channel_config));
}


static void divider_power_on() {
  nrf_drv_gpiote_out_config_t divider_vcc_pin_config = GPIOTE_CONFIG_OUT_SIMPLE(true);
  nrf_drv_gpiote_out_init(PIN_DIVIDER_VCC, &divider_vcc_pin_config);
}

static void divider_power_off() {
  nrf_drv_gpiote_out_uninit(PIN_DIVIDER_VCC);
}


static void on_measurement_timer(void *ctx) {
  divider_power_on();
  double R = sample_ntc_resistance();
  m_measurement_cb(calculate_temp(R));
  divider_power_off();
}


void ntc_init(uint32_t measurement_interval_ms, ntc_measurement_cb_t callback) {
  m_measurement_cb = callback;

  adc_channel_init();
  nrf_drv_gpiote_init();
  divider_power_off();

  app_timer_create(&m_measurement_timer, APP_TIMER_MODE_REPEATED, on_measurement_timer);
  app_timer_start(m_measurement_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);
}
