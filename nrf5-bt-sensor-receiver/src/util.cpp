#include <nrf.h>
#include <nrf_error.h>
#include "util.hpp"

void Util::start_clocks() {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

std::string Util::tohex(const uint8_t *in, size_t insz) {
  const char  *hex = "0123456789ABCDEF";
  std::string out;

  for(size_t i = 0; i < insz; ++i) {
    out += hex[(in[i] >> 4) & 0xF];
    out += hex[in[i] & 0xF];
  }

  return out;
}
