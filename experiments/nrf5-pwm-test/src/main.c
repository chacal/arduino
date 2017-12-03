#include <nrf.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_error.h>
#include <stdlib.h>
#include <nrf_drv_pwm.h>
#include <nrf52.h>
#include <sdk_config_nrf52/sdk_config.h>
#include <app_timer.h>
#include <nrf_pwm.h>

static nrf_drv_pwm_t               m_pwm        = NRF_DRV_PWM_INSTANCE(0);
static nrf_pwm_values_individual_t seq_values[] = {{
                                                       .channel_0 = 1250 | 0x8000,
                                                       .channel_1 = 625 | 0x8000
                                                   }};
static nrf_pwm_sequence_t          seq          = {
    .values.p_individual = seq_values,
    .length = NRF_PWM_VALUES_LENGTH(seq_values)
};

static void clocks_init() {
  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART    = 1;
  while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
}

static void pwm_init() {
  nrf_drv_pwm_config_t pwm_config = NRF_DRV_PWM_DEFAULT_CONFIG;
  APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm, &pwm_config, NULL));
}

static void pwm_start() {
  nrf_drv_pwm_simple_playback(&m_pwm, &seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

static void on_timer(void *ctx) {
  seq_values->channel_0 = 1875 | 0x8000;
  seq_values->channel_1 = 250 | 0x8000;
  NRF_LOG_INFO("Values changed\n");
}

static void timer_init() {
  APP_TIMER_DEF(timer);
  app_timer_create(&timer, APP_TIMER_MODE_REPEATED, on_timer);
  app_timer_start(timer, APP_TIMER_TICKS(3000, 0), NULL);
}

int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  APP_TIMER_INIT(0, 4, false);

  NRF_LOG_INFO("Starting..\n");

  clocks_init();
  pwm_init();
  pwm_start();
  timer_init();

  NRF_LOG_INFO("Started\n");

  for(;;) {
    __WFE();
    __WFI();
  }
}
