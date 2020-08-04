#pragma once


double measure_ntc_resistance(int ntc_pin);

double calculate_temp(double ntc_resistance);

double measure_ntc_temp(int ntc_pin);

void blink(uint8_t times = 1);