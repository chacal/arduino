#include <app_timer.h>
#include "util.hpp"


namespace util {
  seconds calculate_seconds_left(uint32_t start_time_ticks, const seconds &duration_s) {
    uint32_t ticks_in_sec  = APP_TIMER_TICKS(1000);
    uint32_t elapsed_ticks = app_timer_cnt_diff_compute(app_timer_cnt_get(), start_time_ticks);

    uint32_t ticks_left = APP_TIMER_TICKS(1000 * duration_s.get()) - elapsed_ticks;
    uint32_t secs_left  = (ticks_left + (ticks_in_sec / 2)) / ticks_in_sec;  // Round up
    return seconds(secs_left);
  }
}
