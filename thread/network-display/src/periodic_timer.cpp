#include <nrf_log.h>
#include "periodic_timer.hpp"

periodic_timer::periodic_timer(milliseconds period, app_timer_timeout_handler_t on_timer) :
    period(std::chrono::duration_cast<util::rtc_ticks>(period)) {
  APP_ERROR_CHECK(app_timer_create(&app_timer, APP_TIMER_MODE_REPEATED, on_timer));
}

void periodic_timer::start(void *ctx) {
  APP_ERROR_CHECK(app_timer_start(app_timer, period.count(), ctx));
}

void periodic_timer::stop() {
  APP_ERROR_CHECK(app_timer_stop(app_timer));
}
