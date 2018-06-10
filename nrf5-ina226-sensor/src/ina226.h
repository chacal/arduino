#pragma once

typedef void (*ina226_measurement_cb_t)(int16_t raw_measurement, float shunt_voltage_mV, float shunt_current);

void ina226_init(uint32_t measurement_interval_ms, ina226_measurement_cb_t callback);
