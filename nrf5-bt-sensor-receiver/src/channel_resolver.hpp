#pragma once

#include <stdint.h>

namespace channel_resolver{
    typedef enum {
      ADV_CHANNEL_37 = 37,
      ADV_CHANNEL_38 = 38,
      ADV_CHANNEL_39 = 39
    } adv_channel_t;

    uint8_t channel_resolver_get_channel();
    uint8_t channel_resolver_get_next_channel();
    uint8_t channel_resolver_get_frequency(uint8_t channel);
}
