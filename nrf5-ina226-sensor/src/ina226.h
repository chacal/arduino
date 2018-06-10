#pragma once

typedef void (*bme280_measurement_cb_t)(double temperature, double pressure, double humidity);

void bme280_init(uint32_t measurement_interval_ms, bme280_measurement_cb_t callback);
