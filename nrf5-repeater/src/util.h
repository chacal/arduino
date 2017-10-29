#pragma once

#include "radio.h"

typedef struct {
  uint8_t *data;
  uint32_t data_len;
} adv_data_t;

void util_start_clocks();
uint32_t util_adv_report_parse(uint8_t adv_type, nrf_radio_packet_t *adv_report_packet, adv_data_t *adv_data);
void util_init_rand();
uint32_t util_rand_between(uint32_t min, uint32_t max);
