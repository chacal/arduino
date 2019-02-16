#include <hfsm/machine_single.hpp>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <app_scheduler.h>
#include "thread.hpp"
#include "state_machine.hpp"

int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(nullptr));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  APP_ERROR_CHECK(app_timer_init());  // Needs to be initialized before FSM as some states use app_timer in their constructors

  NRF_LOG_INFO("Starting Network Display "
                   APP_VERSION
                   "..")

  state_machine fsm;

  while (true) {
    if (fsm.update()) {
      app_sched_execute();
      thread::run();
    }
  }
}
