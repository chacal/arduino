#pragma once

#include <stdint.h>

void power_manager_init(uint32_t wakeup_pin_number);
void power_manage();
void power_manager_shutdown();