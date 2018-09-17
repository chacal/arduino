#include <hfsm/machine_single.hpp>
#include <nrf_log_default_backends.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include "power_manager.hpp"
#include "display.hpp"
#include "state_machine.hpp"


int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  power_manager::init();

  NRF_LOG_INFO("Starting nrf52-remote-display..")

  state_machine fsm;

  for (;;) {
    if(fsm.update()) {
      power_manager::manage();
    }
  }
}
