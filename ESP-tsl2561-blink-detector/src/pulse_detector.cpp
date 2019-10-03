#include <Arduino.h>

#include "pulse_detector.hpp"
#include "config.hpp"

#define RUNNING_AVG_SAMPLES    200

typedef enum {
  DETECTING_PULSE_START,
  DETECTING_PULSE_END
} detector_state;

static pulse_detector_cb_t m_cb;

void pulse_detector_init(pulse_detector_cb_t cb) {
  m_cb = cb;
}

uint16_t runningAverage(uint16_t val) {
  static int     LM[RUNNING_AVG_SAMPLES];      // LastMeasurements
  static uint8_t index = 0;
  static long    sum   = 0;
  static uint8_t count = 0;

  // keep sum updated to improve speed.
  sum -= LM[index];
  LM[index] = val;
  sum += LM[index];
  index++;
  index = index % RUNNING_AVG_SAMPLES;
  if (count < RUNNING_AVG_SAMPLES) count++;

  return sum / count;
}

void pulse_detector_process(uint16_t adc_value) {
  static unsigned long  m_pulse_start_time = 0;
  static detector_state m_state            = DETECTING_PULSE_START;

  auto avg = runningAverage(adc_value);

  switch (m_state) {
    case DETECTING_PULSE_START:
      if (adc_value > config.pulse_start_coef * avg) {
        // Serial.printf("Pulse start detected. Avg: %u Adc: %u\n", avg, adc_value);
        m_pulse_start_time = millis();
        m_state            = DETECTING_PULSE_END;
      }
      break;
    case DETECTING_PULSE_END:
      if (millis() - m_pulse_start_time > config.max_pulse_length) {
        m_state = DETECTING_PULSE_START;
      } else if (adc_value < config.pulse_end_coef * avg) {
        // Serial.printf("Pulse end detected. Avg: %u Adc: %u\n", avg, adc_value);
        m_cb();
        m_state = DETECTING_PULSE_START;
      }
      break;
  }
}