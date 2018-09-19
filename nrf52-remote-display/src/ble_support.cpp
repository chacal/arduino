#include "ble_support.hpp"
#include <ble.h>
#include <nrf_sdh.h>
#include <nrf_sdh_ble.h>

#define TX_POWER_LEVEL            4
#define BLE_CONN_CFG_TAG          1


namespace ble_support {

  static void ble_stack_init() {
    APP_ERROR_CHECK(nrf_sdh_enable_request());

    // Configure the BLE stack using the default settings in sdk_config.h
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(BLE_CONN_CFG_TAG, &ram_start));

    // Enable BLE stack.
    APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));

    sd_ble_gap_tx_power_set(TX_POWER_LEVEL);

    // Register a handler for BLE events.
    //NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
  }

  void init() {
    ble_stack_init();
  }
}
