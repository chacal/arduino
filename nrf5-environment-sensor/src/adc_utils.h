#pragma once

#include <stdint.h>

void adc_utils_init(uint8_t adc_channel, uint8_t divider_input_pin, uint8_t divider_drive_pin, uint32_t divider_gnd_resistance,
                    bool use_internal_pulldown, bool drive_pin_is_source);

double adc_measure_divider_resistance();
