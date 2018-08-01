#pragma once

#include <stdint.h>

#define VCC_MEASUREMENT_INTERVAL_S          120

typedef void (*vcc_measurement_cb_t)(uint16_t);

void vcc_measurement_init(uint32_t measurement_interval_ms, vcc_measurement_cb_t callback);
