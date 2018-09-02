#pragma once

#include <stdint.h>

namespace channel_resolver {
  enum class adv_ch {
    ch_37 = 37,
    ch_38 = 38,
    ch_39 = 39,
  };

  adv_ch get_next_channel();
  uint8_t get_frequency(adv_ch channel);
}
