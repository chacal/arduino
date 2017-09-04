#pragma once

#include <stddef.h>
#include <ble.h>
#include "config.h"

#define DATA_SERVICE_BASE_UUID                    {{0xA2, 0x49, 0x76, 0xF2, 0xB3, 0x38, 0xD3, 0x8B, 0x40, 0x49, 0xD5, 0x90, 0x00, 0x00, 0x6E, 0x0F}}
#define DATA_SERVICE_UUID_TYPE                    BLE_UUID_TYPE_VENDOR_BEGIN
#define DATA_SERVICE_SERVICE_UUID                 0x0001     // Service UUID = 0f6e000190d549408bd338b3f27649a2
#define DATA_SERVICE_RX_CHARACTERISTIC_UUID       0x0002     // Char UUID    = 0f6e000290d549408bd338b3f27649a2
#define DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN    MAX_BLE_COMMAND_LENGTH


typedef void (*ble_data_service_rx_handler_t)(uint8_t *p_data, uint16_t length);

void ble_data_service_init(ble_data_service_rx_handler_t rx_handler);
void ble_data_service_on_ble_evt(ble_evt_t *p_ble_evt);
