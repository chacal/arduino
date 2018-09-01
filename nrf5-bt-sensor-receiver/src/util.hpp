#pragma once

#include <stdint.h>

extern "C" {
#include "radio.h"
}

typedef struct {
  uint8_t *data;
  uint32_t data_len;
} adv_data_t;

namespace Util {
  void startClocks();
  uint32_t parseAdvReport(uint8_t adv_type, nrf_radio_packet_t *adv_report_packet, adv_data_t *adv_data);
}

