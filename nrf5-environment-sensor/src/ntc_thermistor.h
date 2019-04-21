#pragma once

#include <stdint.h>

typedef void (*ntc_measurement_cb_t)(double temperature);

void ntc_init(uint32_t measurement_interval_ms, ntc_measurement_cb_t callback);
