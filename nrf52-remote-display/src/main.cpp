#include <hfsm/machine_single.hpp>
#include <nrf_log_default_backends.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include "power_manager.hpp"
#include "display.hpp"
#include "state_machine.hpp"
#include "ble_support.hpp"
#include "ble_data_service.hpp"
#include "ble_support_adv.hpp"


int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("Starting nrf52-remote-display..")

  APP_ERROR_CHECK(app_timer_init());
  power_manager::init();
  ble_support::init();
  ble_data_service::init();
  ble_support::adv::init();

  state_machine fsm;

  for (;;) {
    if(fsm.update()) {
      power_manager::manage();
    }
  }
}
