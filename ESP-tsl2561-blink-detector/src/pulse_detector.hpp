#pragma once

#include <cstdint>

typedef void (*pulse_detector_cb_t)();


void pulse_detector_init(pulse_detector_cb_t cb);

void pulse_detector_process(uint16_t adc_value);