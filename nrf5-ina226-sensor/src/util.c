#include <nrf.h>
#include <device/nrf.h>
#include "util.h"

void util_start_clocks() {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 1);

  NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_LFCLKSTART    = 1;
  while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 1);
}

