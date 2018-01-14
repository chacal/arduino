#pragma once

typedef void (*ble_dfu_trigger_service_cb_t)();

void ble_dfu_trigger_service_init(const char *device_name, ble_dfu_trigger_service_cb_t dfu_trigger_cb);
