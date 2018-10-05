#include <hfsm/machine_single.hpp>
#include <nrf_log_default_backends.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_timer.h>
#include <app_scheduler.h>
#include "power_manager.hpp"
#include "display.hpp"
#include "state_machine.hpp"
#include "ble_support.hpp"
#include "ble_data_service.hpp"
#include "ble_support_adv.hpp"

#define APP_SCHEDULER_QUEUE_SIZE 20

int main() {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();
  NRF_LOG_INFO("Starting nrf52-remote-display..")
  APP_SCHED_INIT(APP_TIMER_SCHED_EVENT_DATA_SIZE, APP_SCHEDULER_QUEUE_SIZE);
  APP_ERROR_CHECK(app_timer_init());

  display       d;
  state_machine fsm(d);
  auto          ble_evt_handler = [&](const ble_evt_t *p_ble_evt) { fsm.react(p_ble_evt); };
  auto          rx_data_handler = [&](ble_data_service::rx_data &received_data) { fsm.react(received_data); };

  power_manager::init();
  ble_support::init(ble_evt_handler);
  ble_data_service::init(rx_data_handler);
  ble_support::adv::init();

  for (;;) {
    app_sched_execute();
    if (fsm.update()) {
      power_manager::manage();
    }
  }
}
