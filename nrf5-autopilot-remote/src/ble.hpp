#pragma once

void ble_init(const char *device_name);

void ble_sensor_advertising_stop();

void ble_sensor_advertising_start();

void ble_sensor_advertising_init(uint8_t *manuf_data, uint8_t manuf_data_len);

uint32_t ble_sensor_advertising_crc32(uint8_t *manuf_data, uint8_t manuf_data_len);