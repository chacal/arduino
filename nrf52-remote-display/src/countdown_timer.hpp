#pragma once

#include <chrono>
#include <app_timer.h>
#include "util.hpp"

using namespace std::chrono;

class countdown_timer {
public:

  countdown_timer(seconds timeout, milliseconds period, app_timer_timeout_handler_t on_timer);

  void start(void *ctx);

  void stop();

  seconds seconds_left();

private:
  app_timer_t          app_timer_data = {{0}};
  const app_timer_id_t app_timer      = &app_timer_data;
  seconds              timeout;
  util::rtc_ticks      start_time;
  util::rtc_ticks      period;
};