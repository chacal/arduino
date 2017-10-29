#include <nrf.h>
#include <nrf_error.h>
#include <stdlib.h>
#include "util.h"
#include "radio.h"

void util_start_clocks() {
  NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
  NRF_CLOCK->TASKS_HFCLKSTART    = 1;
  while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
}

uint32_t util_adv_report_parse(uint8_t adv_type, nrf_radio_packet_t *adv_report_packet, adv_data_t *adv_data) {
  uint32_t index = 0;
  uint8_t *p_data = &adv_report_packet->payload[6];  // Skip first 6 bytes of adv report (they are a BLE MAC address)
  uint8_t adv_data_length = adv_report_packet->payload_length - 6;


  while(index < adv_data_length) {
    uint8_t field_length = p_data[index];
    uint8_t field_type   = p_data[index + 1];

    if(field_type == adv_type) {
      adv_data->data   = &p_data[index + 2];
      adv_data->data_len = field_length - 1;
      return NRF_SUCCESS;
    }
    index += field_length + 1;
  }
  return NRF_ERROR_NOT_FOUND;
}

uint32_t util_rand_between(uint32_t min, uint32_t max) {
  return rand() % (max + 1 - min) + min;
}
