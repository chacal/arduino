#pragma once

#include <cstdint>

typedef void (*vcc_measurement_cb_t)(uint16_t);

void vcc_measurement_init(vcc_measurement_cb_t callback);

void sample_vcc();