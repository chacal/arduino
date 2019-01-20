#include <hfsm/machine_single.hpp>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include "thread.hpp"
#include "state_machine.hpp"

int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();


  NRF_LOG_INFO("Starting Network Display "
                   APP_VERSION
                   "..")

  state_machine fsm;

  thread::initialize();

  while (true) {
    if (fsm.update()) {
      thread::run();
    }
  }
}
