#pragma once

#include <nrf_sdh_ble.h>

namespace ble_support {
  void init(nrf_sdh_ble_evt_handler_t ble_evt_handler, void *ctx);
}