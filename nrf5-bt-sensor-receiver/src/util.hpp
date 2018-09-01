#pragma once

#include <stdint.h>
#include <vector>

extern "C" {
#include "radio.h"
}

namespace Util {
  void startClocks();
  std::vector<uint8_t> getAdvPacketField(uint8_t adv_type, nrf_radio_packet_t *adv_report_packet);
}

