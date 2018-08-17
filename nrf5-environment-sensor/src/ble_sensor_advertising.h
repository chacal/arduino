#pragma once

#include "ble_config.h"

void ble_sensor_advertising_init(void *manuf_data, uint8_t manuf_data_len);
void ble_sensor_advertising_start(uint16_t adv_interval);
void ble_sensor_advertising_stop();
