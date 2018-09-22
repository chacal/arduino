#pragma once

#include <functional>
#include <nrf_sdh_ble.h>


namespace ble_support {
  using ble_evt_handler_t = std::function<void(ble_evt_t const *p_ble_evt)>;

  void init(const ble_evt_handler_t &ble_evt_handler);
}