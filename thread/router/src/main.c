#include <openthread/thread.h>
#include <thread_utils.h>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <coap_dfu.h>
#include <openthread/platform/alarm-milli.h>
#include <app_timer.h>
#include <mem_manager.h>
#include <nrfx_gpiote.h>

#include "thread.h"

#define USE_ANTENNA_SWITCH     true
#define USE_EXTERNAL_ANTENNA   true

void configure_antenna() {
  if (USE_ANTENNA_SWITCH) {
    nrfx_gpiote_init();
    nrfx_gpiote_out_config_t high_config = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(true);
    nrfx_gpiote_out_config_t low_config  = NRFX_GPIOTE_CONFIG_OUT_SIMPLE(false);
    if (USE_EXTERNAL_ANTENNA) {
      nrfx_gpiote_out_init(24, &low_config);
      nrfx_gpiote_out_init(25, &high_config);
    } else {
      nrfx_gpiote_out_init(25, &low_config);
      nrfx_gpiote_out_init(24, &high_config);
    }
  }
}

int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  APP_ERROR_CHECK(nrf_mem_init());
  APP_ERROR_CHECK(app_timer_init());
  configure_antenna();

  NRF_LOG_INFO("Starting Thread Router " APP_VERSION "..")

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
