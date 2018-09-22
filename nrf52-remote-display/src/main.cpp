#include <hfsm/machine_single.hpp>
#include <nrf_log_default_backends.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_timer.h>
#include "power_manager.hpp"
#include "display.hpp"
#include "state_machine.hpp"
#include "ble_support.hpp"
#include "ble_data_service.hpp"
#include "ble_support_adv.hpp"

static void pass_ble_events_to_fsm(const ble_evt_t *p_ble_evt, void *ctx) {
  static_cast<state_machine*>(ctx)->react(p_ble_evt);
}

int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("Starting nrf52-remote-display..")

  display d;
  state_machine fsm(d);

  APP_ERROR_CHECK(app_timer_init());
  power_manager::init();
  ble_support::init(pass_ble_events_to_fsm, &fsm);
  ble_data_service::init();
  ble_support::adv::init();

  for (;;) {
    if(fsm.update()) {
      power_manager::manage();
    }
  }
}
