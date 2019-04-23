#pragma once

#define APP_BLE_CFG_TAG                           1

#define DEFAULT_ADV_INTERVAL                      MSEC_TO_UNITS(10000, UNIT_0_625_MS)
#define ADV_TIMEOUT                               0

#define MIN_CONN_INTERVAL                         MSEC_TO_UNITS(100, UNIT_1_25_MS)
#define MAX_CONN_INTERVAL                         MSEC_TO_UNITS(500, UNIT_1_25_MS)
#define SLAVE_LATENCY                             2
#define CONN_SUP_TIMEOUT                          MSEC_TO_UNITS(10000, UNIT_10_MS)
#define TX_POWER_LEVEL                            4     // Max power, +4dBm


#define DATA_SERVICE_BASE_UUID                    {{0x7E, 0x64, 0x1F, 0x11, 0xF9, 0xC2, 0xA5, 0xBD, 0xE5, 0x4B, 0xDC, 0xC6, 0x00, 0x00, 0xCA, 0x61}}
#define DATA_SERVICE_UUID_TYPE                    BLE_UUID_TYPE_VENDOR_BEGIN
#define DATA_SERVICE_SERVICE_UUID                 0x0001     // Service UUID = 61ca0001c6dc4be5bda5c2f9111f647e
#define DATA_SERVICE_RX_CHARACTERISTIC_UUID       0x0002     // Char UUID    = 61ca0002c6dc4be5bda5c2f9111f647e
#define DATA_SERVICE_RX_CHARACTERISTIC_MAX_LEN    16

