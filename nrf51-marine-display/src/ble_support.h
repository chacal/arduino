#pragma once
#include <ble_advertising.h>
#include <softdevice_handler.h>

void ble_support_init(ble_evt_handler_t ble_evt_handler);
void ble_support_advertising_init();
void ble_support_advertising_start();
