/* Copyright (c) Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 *   3. Neither the name of Nordic Semiconductor ASA nor the names of other
 *   contributors to this software may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 *   4. This software must only be used in a processor manufactured by Nordic
 *   Semiconductor ASA, or in a processor manufactured by a third party that
 *   is used in combination with a processor manufactured by Nordic Semiconductor.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "channel_resolver.hpp"
#include "nrf_gpio.h"

namespace channel_resolver{

    typedef struct {
      adv_channel_t channels[3];
      uint8_t       n_channels;
    } adv_channel_hop_sequence_t;

    static adv_channel_hop_sequence_t hop_sequence = {{ ADV_CHANNEL_37, ADV_CHANNEL_38, ADV_CHANNEL_39 }, 3 };

    /* Returns the logical channel corresponding to the current radio frequency */
    uint8_t get_channel() {
      uint8_t freq = NRF_RADIO->FREQUENCY;

      uint8_t channel;
      /* Special cases for the advertise channels */
      if(freq == 2) {
        channel = 37;
      } else if(freq == 26) {
        channel = 38;
      } else if(freq == 80) {
        channel = 39;
        /* Data channels */
      } else {
        channel = (freq / 2) - (freq < 26 ? 2 : 3);
      } // Spec Vol. 6, Part B, 1.4.1

      return channel;
    }

    int get_index_in_hop_sequence(adv_channel_t channel) {
      int i;
      for(i = 0; (i < hop_sequence.n_channels) && (channel != hop_sequence.channels[i]); i++) {}
      return i;
    }


    uint8_t get_next_channel() {
      adv_channel_t current_channel = (adv_channel_t) get_channel();
      int           current_index   = get_index_in_hop_sequence(current_channel);
      int           next_index      = (current_index + 1) % hop_sequence.n_channels;
      return (uint8_t) hop_sequence.channels[next_index];
    }

    uint8_t get_frequency(uint8_t channel) {
      uint8_t freq;

      /* Special cases for the advertise channels */
      if(channel == 37) {
        freq = 2;
      } else if(channel == 38) {
        freq = 26;
      } else if(channel == 39) {
        freq = 80;
      } else {
        freq = channel + (channel < 11 ? 2 : 3) * 2;
      } // Spec Vol. 6, Part B, 1.4.1

      return freq;
    }
}
