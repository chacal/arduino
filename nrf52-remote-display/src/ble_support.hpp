#pragma once

#include <nrf_sdh_ble.h>


namespace ble_support {
  struct ble_observer_holder {
    nrf_sdh_ble_evt_handler_t handler;
    void *ctx;
  };

  void init(nrf_sdh_ble_evt_handler_t ble_evt_handler, void *ctx);
}