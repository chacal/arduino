#pragma once

#include <nrf_drv_twi.h>

typedef enum {
  BMP180_MEASUREMENT_TYPE_TEMPERATURE,
  BMP180_MEASUREMENT_TYPE_PRESSURE
} bmp180_measurement_type_t;


void bmp180_init(nrf_drv_twi_t *twi);
void bmp180_get_temp_and_pressure();
