#pragma once

#include <ble.h>

typedef void (*ble_serial_link_rx_handler_t)(uint8_t *p_data, uint16_t length);

void ble_serial_link_init(ble_serial_link_rx_handler_t rx_handler);
void ble_serial_link_on_ble_evt(ble_evt_t *p_ble_evt);

uint32_t ble_serial_link_string_send(uint8_t *p_string, size_t length);