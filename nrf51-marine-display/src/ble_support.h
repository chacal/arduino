#pragma once
#include <softdevice_handler.h>

#define BLE_GAP_LESC_P256_SK_LEN 32
typedef struct {
  uint8_t   sk[BLE_GAP_LESC_P256_SK_LEN];        /**< LE Secure Connections Elliptic Curve Diffie-Hellman P-256 Private Key in little-endian. */
} ble_gap_lesc_p256_sk_t;

typedef void (*ble_support_callback_t)();


void ble_support_init(ble_evt_handler_t ble_evt_handler);
void ble_support_advertising_init();
void ble_support_advertising_start();
void ble_support_disconnect(ble_support_callback_t callback);
void ble_support_start_discoverable();
void ble_support_factory_reset();
