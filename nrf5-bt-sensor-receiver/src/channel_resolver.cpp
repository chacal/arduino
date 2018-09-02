#include <vector>
#include <algorithm>
#include <iterator>


#include "channel_resolver.hpp"
#include "nrf_gpio.h"

namespace channel_resolver {

  static std::vector<adv_ch> hop_sequence = {adv_ch::ch_37, adv_ch::ch_38, adv_ch::ch_39};

  /* Returns the logical channel corresponding to the current radio frequency */
  adv_ch get_current_adv_channel() {
    uint8_t freq = NRF_RADIO->FREQUENCY;

    switch(freq) {
      case 2:
        return adv_ch::ch_37;
      case 26:
        return adv_ch::ch_38;
      case 80:
        return adv_ch::ch_39;
      default:
        return adv_ch::ch_37;  // Should not happen as we are only listening on advertisement channels
    }
  }

  int get_index_in_hop_sequence(adv_ch channel) {
    return std::distance(hop_sequence.begin(), find(hop_sequence.begin(), hop_sequence.end(), channel));
  }

  adv_ch get_next_channel() {
    auto current_channel = get_current_adv_channel();
    int  current_index   = get_index_in_hop_sequence(current_channel);
    int  next_index      = (current_index + 1) % hop_sequence.size();
    return hop_sequence[next_index];
  }

  uint8_t get_frequency(adv_ch channel) {
    switch(channel) {
      case adv_ch::ch_37:
        return 2;
      case adv_ch::ch_38:
        return 26;
      case adv_ch::ch_39:
        return 80;
      default:
        return 2;  // Should not happen as we are only listening on advertisement channels
    }
  }
}
