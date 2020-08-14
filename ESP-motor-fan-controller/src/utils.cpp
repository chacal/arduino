#include <Arduino.h>
#include "utils.hpp"

#define THERMISTOR_RESISTANCE                 10000
#define THERMISTOR_DIVIDER_GND_RESISTANCE     10000
#define THERMISTOR_BETA_COEF                   3950     // From thermistor's datasheet
#define THERMISTOR_NOMINAL_TEMP                  25     // From thermistor's datasheet (25°C)

#define ADC_AVERAGING_COUNT                      20

double measure_ntc_resistance(int ntc_pin) {
  uint32_t val = 0;

  for (int i = 0; i < ADC_AVERAGING_COUNT; ++i) {
    val += analogRead(ntc_pin);
  }

  auto avg_val = val / (float) ADC_AVERAGING_COUNT;
  return THERMISTOR_DIVIDER_GND_RESISTANCE * (1024.0f / avg_val - 1);
}

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

double measure_ntc_temp(int ntc_pin) {
  return calculate_temp(measure_ntc_resistance(ntc_pin));
}

void blink(uint8_t times) {
  for (int i = 0; i < times; ++i) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(5);
    digitalWrite(LED_BUILTIN, HIGH);
    if (times > 1) {
      delay(50);
    }
  }
}
