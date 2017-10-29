#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void (*tx_timer_handler_t)();

void tx_timer_init(tx_timer_handler_t);
void tx_timer_start_with_delay(uint32_t delay);

uint32_t tx_timer_get_current_delay();
void tx_timer_set_delay(uint32_t delay);

bool tx_timer_stopped();
uint32_t tx_timer_time_left();
