#pragma once

void ble_support_init(const char *device_name);
void ble_support_advertising_init(void *manuf_data, uint8_t manuf_data_len);
void ble_support_advertising_start();
