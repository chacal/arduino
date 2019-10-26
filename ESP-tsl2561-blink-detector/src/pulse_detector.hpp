#pragma once

#include <cstdint>

typedef void (*pulse_detector_cb_t)();


void pulse_detector_init(pulse_detector_cb_t pulse_detected_cb, pulse_detector_cb_t full_buffer_cb);

void pulse_detector_process(uint16_t adc_value);

uint16_t pulse_detector_current_avg();

void pulse_detector_write_samples(Print &p);