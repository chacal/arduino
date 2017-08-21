#pragma once
#include <ble_advertising.h>
#include <softdevice_handler.h>


void gap_params_init();

void ble_stack_init(ble_evt_handler_t ble_evt_handler);
void on_ble_evt(ble_evt_t * p_ble_evt);

void advertising_init();
void on_adv_evt(ble_adv_evt_t ble_adv_evt);
