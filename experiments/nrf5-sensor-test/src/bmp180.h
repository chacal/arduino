#pragma once

#include <nrf_drv_twi.h>

typedef void (*bmp180_measurement_cb_t)(double temperature, double pressure);

void bmp180_init(uint32_t measurement_interval_ms, bmp180_measurement_cb_t callback);
