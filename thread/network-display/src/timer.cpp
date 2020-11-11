#include <nrf_log.h>
#include "timer.hpp"

timer::timer(milliseconds period, app_timer_mode_t mode, app_timer_timeout_handler_t on_timer) :
    period(std::chrono::duration_cast<util::rtc_ticks>(period)) {
  APP_ERROR_CHECK(app_timer_create(&app_timer, mode, on_timer));
}

void timer::start(void *ctx) {
  APP_ERROR_CHECK(app_timer_start(app_timer, period.count(), ctx));
}

void timer::stop() {
  APP_ERROR_CHECK(app_timer_stop(app_timer));
}
