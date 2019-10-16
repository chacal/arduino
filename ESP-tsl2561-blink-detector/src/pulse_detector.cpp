#include <Arduino.h>
#include <CircularBuffer.h>

#include "pulse_detector.hpp"
#include "config.hpp"

#define RUNNING_AVG_SAMPLES     30
#define SAMPLE_BUFFER_SIZE     700

typedef enum {
  DETECTING_PULSE_START,
  DETECTING_PULSE_END
} detector_state;

static pulse_detector_cb_t                          m_pulse_detected_cb;
static pulse_detector_cb_t                          m_full_buffer_cb;
static uint16_t                                     current_avg;
static CircularBuffer<uint16_t, SAMPLE_BUFFER_SIZE> buffer;

void pulse_detector_init(pulse_detector_cb_t pulse_detected_cb, pulse_detector_cb_t full_buffer_cb) {
  m_pulse_detected_cb = pulse_detected_cb;
  m_full_buffer_cb    = full_buffer_cb;
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
  static uint16_t       adc_counter        = 0;

  buffer.push(adc_value);
  current_avg = runningAverage(adc_value);
  adc_counter++;

  switch (m_state) {
    case DETECTING_PULSE_START:
      if (adc_value > config.pulse_start_coef * current_avg) {
        // Serial.printf("Pulse start detected. Avg: %u Adc: %u\n", avg, adc_value);
        m_pulse_start_time = millis();
        m_state            = DETECTING_PULSE_END;
      }
      break;
    case DETECTING_PULSE_END:
      if (millis() - m_pulse_start_time > config.max_pulse_length) {
        m_state = DETECTING_PULSE_START;
      } else if (adc_value < config.pulse_end_coef * current_avg) {
        // Serial.printf("Pulse end detected. Avg: %u Adc: %u\n", avg, adc_value);
        m_pulse_detected_cb();
        m_state = DETECTING_PULSE_START;
      }
      break;
  }

  if (adc_counter == SAMPLE_BUFFER_SIZE) {
    m_full_buffer_cb();
    adc_counter = 0;
  }
}

uint16_t pulse_detector_current_avg() {
  return current_avg;
}

void pulse_detector_write_samples(Print &p) {
  noInterrupts();
  using index_t = decltype(buffer)::index_t;
  for (index_t i = 0; i < buffer.size(); i++) {
    p.write(buffer[i] & 0xFF);
    p.write(buffer[i] >> 8);
  }
  interrupts();
}