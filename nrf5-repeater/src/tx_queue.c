#include <memory.h>
#include <nrf_error.h>
#include <stdint.h>
#include "tx_queue.h"

#define TX_QUEUE_SIZE  10

static tx_queue_element_t tx_queue[TX_QUEUE_SIZE];

uint32_t tx_queue_add_packet(nrf_radio_packet_t *packet, uint32_t delay_us) {
  for(int i = 0; i < TX_QUEUE_SIZE; ++i) {
    if(!tx_queue[i].in_use) {
      tx_queue[i].in_use = true;
      tx_queue[i].delay_us = delay_us;
      memcpy(&tx_queue[i].tx_packet, packet, sizeof(packet));
      return NRF_SUCCESS;
    }
  }
  return NRF_ERROR_NO_MEM;
}

uint32_t tx_queue_remove_first_expired_element(tx_queue_element_t *element) {
  for(int i = 0; i < TX_QUEUE_SIZE; ++i) {
    if(tx_queue[i].in_use && tx_queue[i].delay_us == 0) {
      memcpy(element, &tx_queue[i], sizeof(tx_queue[i]));
      tx_queue[i].in_use = false;
      return NRF_SUCCESS;
    }
  }
  return NRF_ERROR_NOT_FOUND;
}

uint32_t tx_queue_get_delay_for_next(uint32_t *delay_for_next) {
  bool found = false;
  *delay_for_next = UINT32_MAX;

  for(int i = 0; i < TX_QUEUE_SIZE; ++i) {
    if(tx_queue[i].in_use && tx_queue[i].delay_us < *delay_for_next) {
      *delay_for_next = tx_queue[i].delay_us;
      found = true;
    }
  }

  return found ? NRF_SUCCESS : NRF_ERROR_NOT_FOUND;
}

void tx_queue_update_delays(uint32_t elapsed_us) {
  for(int i = 0; i < TX_QUEUE_SIZE; ++i) {
    if(tx_queue[i].in_use && tx_queue[i].delay_us > elapsed_us) {
      tx_queue[i].delay_us -= elapsed_us;
    } else {
      tx_queue[i].delay_us = 0;
    }
  }
}
