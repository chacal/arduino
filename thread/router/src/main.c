#include <openthread/thread.h>
#include <thread_utils.h>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <coap_dfu.h>
#include <openthread/platform/alarm-milli.h>
#include <app_timer.h>
#include <mem_manager.h>

#include "thread.h"

int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  APP_ERROR_CHECK(nrf_mem_init());
  APP_ERROR_CHECK(app_timer_init());

  NRF_LOG_INFO("Starting Thread Router..")

  while (true) {
    thread_initialize();
    APP_ERROR_CHECK(coap_dfu_init(thread_ot_instance_get()));

    uint32_t now, before = otPlatAlarmMilliGetNow();

    while (!thread_soft_reset_was_requested()) {
      coap_dfu_process();
      thread_process();
      thread_sleep();
      now = otPlatAlarmMilliGetNow();
      if (now - before > 1000)
      {
        coap_time_tick();
        before = now;
      }
    }

    thread_soft_deinit();
  }
}
