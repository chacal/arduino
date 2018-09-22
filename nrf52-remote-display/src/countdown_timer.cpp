#include "countdown_timer.hpp"

countdown_timer::countdown_timer(seconds timeout, milliseconds period, app_timer_timeout_handler_t on_timer) :
    timeout(timeout),
    period(std::chrono::duration_cast<util::rtc_ticks>(period)) {
  app_timer_create(&app_timer, APP_TIMER_MODE_REPEATED, on_timer);
}

void countdown_timer::start(void *ctx) {
  app_timer_start(app_timer, period.count(), ctx);
  start_time = util::rtc_ticks(app_timer_cnt_get());
}

void countdown_timer::stop() {
  app_timer_stop(app_timer);
}

seconds countdown_timer::seconds_left() {
  auto elapsed   = util::rtc_ticks(app_timer_cnt_diff_compute(app_timer_cnt_get(), start_time.count()));
  auto time_left = timeout - elapsed;
  return std::chrono::ceil<std::chrono::seconds>(time_left);
}