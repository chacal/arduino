#include <app_timer.h>
#include <nrf_drv_gpiote.h>
#include <nrf_log.h>
#include <nrf_drv_saadc.h>
#include <math.h>

#include "ntc_thermistor.h"
#include "adc_utils.h"

/*
 * Wiring:
 * - 100k NTC thermistor between PIN_DIVIDER_VCC and PIN_DIVIDER_MEAS
 * - (internal 160k resistor is used between PIN_DIVIDER_MEAS and GND to create voltage divider)
 */

#define DIVIDER_INPUT_PIN      NRF_SAADC_INPUT_AIN0     // == Pin P0.02, Temperature is measured from this pin
#define DIVIDER_DRIVE_PIN                         3     // This set to HIGH when measuring temperature
#define DIVIDER_GND_RESISTOR                 160000     // nRF's internal resistor ladder's resistance, 160k. See PS, page 390
#define ADC_CHANNEL                               1     // ADC channel to use, must be different than VCC measurement channel

#define THERMISTOR_RESISTANCE              100000.0     // NTC thermistor nominal value
#define THERMISTOR_BETA_COEF                   3950     // From thermistor's datasheet
#define THERMISTOR_NOMINAL_TEMP                  25     // From thermistor's datasheet (25°C)


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


static void on_measurement_timer(void *ctx) {
  double R = adc_measure_divider_resistance();
  m_measurement_cb(calculate_temp(R));
}

void ntc_init(uint32_t measurement_interval_ms, ntc_measurement_cb_t callback) {
  m_measurement_cb = callback;

  adc_utils_init(ADC_CHANNEL, DIVIDER_INPUT_PIN, DIVIDER_DRIVE_PIN, DIVIDER_GND_RESISTOR, true);

  app_timer_create(&m_measurement_timer, APP_TIMER_MODE_REPEATED, on_measurement_timer);
  app_timer_start(m_measurement_timer, APP_TIMER_TICKS(measurement_interval_ms), NULL);
}
