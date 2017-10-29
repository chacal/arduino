#pragma once

#include <sched.h>
#include <stdbool.h>
#include "radio.h"

typedef struct {
  bool in_use;
  uint32_t delay_us;
  nrf_radio_packet_t tx_packet;
} tx_queue_element_t;


uint32_t tx_queue_add_packet(nrf_radio_packet_t *packet, uint32_t delay_us);
uint32_t tx_queue_remove_first_expired_element(tx_queue_element_t *element);
uint32_t tx_queue_get_delay_for_next(uint32_t *delay_for_next);
void tx_queue_update_delays(uint32_t elapsed_us);
