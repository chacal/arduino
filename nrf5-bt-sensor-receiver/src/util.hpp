#pragma once

#include <stdint.h>
#include <vector>
#include <optional>

extern "C" {
#include "radio.h"
}

namespace Util {
  void start_clocks();
  std::optional<std::vector<uint8_t>> getAdvPacketField(uint8_t adv_type, const nrf_packet_data *adv_report_packet);
  std::string tohex(const uint8_t *in, size_t insz);
}

