#pragma once

typedef void (*ina226_measurement_cb_t)(int16_t raw_measurement, double shunt_voltage_mV, double shunt_current_A, double bus_voltage_mV);

void ina226_init(uint32_t measurement_interval_ms, double shunt_resistance, double i_max_expected, ina226_measurement_cb_t callback);
