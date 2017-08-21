#pragma once
#include "softdevice_handler.h"

void ble_stack_init(ble_evt_handler_t ble_evt_handler);
void on_ble_evt(ble_evt_t * p_ble_evt);
