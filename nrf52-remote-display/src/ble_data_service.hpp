#pragma once

#include <functional>
#include <vector>
#include <ble.h>
#include "config.hpp"

#define DATA_SERVICE_BASE_UUID                    {{0xA2, 0x49, 0x76, 0xF2, 0xB3, 0x38, 0xD3, 0x8B, 0x40, 0x49, 0xD5, 0x90, 0x00, 0x00, 0x6E, 0x0F}}
#define DATA_SERVICE_UUID_TYPE                    BLE_UUID_TYPE_VENDOR_BEGIN
#define DATA_SERVICE_SERVICE_UUID                 0x0001     // Service UUID = 0f6e000190d549408bd338b3f27649a2
#define DATA_SERVICE_RX_CHARACTERISTIC_UUID       0x0002     // Char UUID    = 0f6e000290d549408bd338b3f27649a2
#define DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN    500        // Max length for JSON input

namespace ble_data_service {
  struct rx_data {
    const uint8_t *data;
    uint16_t      len;
  };

  using rx_data_handler = std::function<void(const rx_data &received_data)>;

  void init(const rx_data_handler &data_handler);
}
