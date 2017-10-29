#pragma once

#define RADIO_PACKET_PAYLOAD_LENGTH  37

typedef struct {
  uint8_t s0;
  uint8_t payload_length;
  uint8_t s1;
  uint8_t payload[RADIO_PACKET_PAYLOAD_LENGTH];
} nrf_radio_packet_t;

typedef void (*radio_packet_cb_t)(nrf_radio_packet_t);

void radio_init(radio_packet_cb_t on_rx_adv_packet);
void radio_rx_start();
void radio_send_packet(nrf_radio_packet_t *packet);
void radio_enable_irq();
void radio_disable_irq();
