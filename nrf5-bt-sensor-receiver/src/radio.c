#include <nrf.h>
#include "radio.h"
#include "channel_resolver.h"

#define RADIO_TIMER                       NRF_TIMER2
#define RADIO_TIMER_RX_TIMEOUT_COMPARE    0
#define PPI_CH_TIMER_START                0
#define PPI_CH_RX_TIMEOUT                 1

#define RX_TIMEOUT_US                 50000
#define PACKET_TYPE_ADV_NONCONN_IND    0x02

static nrf_radio_packet_t m_rx_tx_buf;
static radio_packet_cb_t  m_on_rx_adv_packet;

static void clear_events() {
  NRF_RADIO->EVENTS_DISABLED = 0;
  NRF_RADIO->EVENTS_END      = 0;
  NRF_RADIO->EVENTS_READY    = 0;
}

void radio_init(radio_packet_cb_t on_rx_adv_packet) {
  m_on_rx_adv_packet = on_rx_adv_packet;

  /* Enable power to RADIO */
  NRF_RADIO->POWER = 1;

  /* Set radio transmit power to 4dBm */
  NRF_RADIO->TXPOWER = (RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos);

  /* Set radio mode to 1Mbit/s Bluetooth Low Energy */
  NRF_RADIO->MODE = (RADIO_MODE_MODE_Ble_1Mbit << RADIO_MODE_MODE_Pos);

  /* Set the requested channel */
  NRF_RADIO->FREQUENCY = channel_resolver_get_frequency(ADV_CHANNEL_37);

  /* This value needs to correspond to the channel being used */
  NRF_RADIO->DATAWHITEIV = ADV_CHANNEL_37;

  /* Configure Access Address according to the BLE standard */
  NRF_RADIO->PREFIX0 = 0x8e;
  NRF_RADIO->BASE0   = 0x89bed600;

  /* Use logical address 0 (prefix0 + base0) = 0x8E89BED6 when transmitting and receiving */
  NRF_RADIO->TXADDRESS   = 0x00;
  NRF_RADIO->RXADDRESSES = 0x01;

  /* PCNF-> Packet Configuration.
   * We now need to configure the sizes S0, S1 and length field to match the
   * datapacket format of the advertisement packets.
   */
  NRF_RADIO->PCNF0 = (
      (((1UL) << RADIO_PCNF0_S0LEN_Pos) & RADIO_PCNF0_S0LEN_Msk) |  /* Length of S0 field in bytes 0-1.    */
      (((2UL) << RADIO_PCNF0_S1LEN_Pos) & RADIO_PCNF0_S1LEN_Msk) |  /* Length of S1 field in bits 0-8.     */
      (((6UL) << RADIO_PCNF0_LFLEN_Pos) & RADIO_PCNF0_LFLEN_Msk)    /* Length of length field in bits 0-8. */
  );

  /* Packet configuration */
  NRF_RADIO->PCNF1 = (
      (((37UL) << RADIO_PCNF1_MAXLEN_Pos) & RADIO_PCNF1_MAXLEN_Msk) |                      /* Maximum length of payload in bytes [0-255] */
      (((0UL) << RADIO_PCNF1_STATLEN_Pos) & RADIO_PCNF1_STATLEN_Msk) |                      /* Expand the payload with N bytes in addition to LENGTH [0-255] */
      (((3UL) << RADIO_PCNF1_BALEN_Pos) & RADIO_PCNF1_BALEN_Msk) |                      /* Base address length in number of bytes. */
      (((RADIO_PCNF1_ENDIAN_Little) << RADIO_PCNF1_ENDIAN_Pos) & RADIO_PCNF1_ENDIAN_Msk) |  /* Endianess of the S0, LENGTH, S1 and PAYLOAD fields. */
      (((1UL) << RADIO_PCNF1_WHITEEN_Pos) & RADIO_PCNF1_WHITEEN_Msk)                         /* Enable packet whitening */
  );

  /* CRC config */
  NRF_RADIO->CRCCNF  = (RADIO_CRCCNF_LEN_Three << RADIO_CRCCNF_LEN_Pos) |
                       (RADIO_CRCCNF_SKIPADDR_Skip << RADIO_CRCCNF_SKIPADDR_Pos); /* Skip Address when computing CRC */
  NRF_RADIO->CRCINIT = 0x555555;                                                  /* Initial value of CRC */
  NRF_RADIO->CRCPOLY = 0x00065B;                                                  /* CRC polynomial function */

  clear_events();

  NRF_RADIO->INTENCLR = 0xFFFFFFFF;
  NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk | RADIO_INTENSET_DISABLED_Msk;

  /* Enable RADIO interrupts */
  NVIC_SetPriority(RADIO_IRQn, 2);
  NVIC_ClearPendingIRQ(RADIO_IRQn);
  NVIC_EnableIRQ(RADIO_IRQn);

  RADIO_TIMER->PRESCALER = 4;  // 1MHz, 1 tick == 1µs
  RADIO_TIMER->BITMODE   = TIMER_BITMODE_BITMODE_16Bit;  // Max timer length == ~65ms
  RADIO_TIMER->SHORTS    = TIMER_SHORTS_COMPARE0_CLEAR_Msk | TIMER_SHORTS_COMPARE0_STOP_Msk;  // Clear & stop timer on compare0

  // Start radio timer always when radio gets ready
  NRF_PPI->CH[PPI_CH_TIMER_START].EEP = (uint32_t) &NRF_RADIO->EVENTS_READY;
  NRF_PPI->CH[PPI_CH_TIMER_START].TEP = (uint32_t) &RADIO_TIMER->TASKS_START;

  NRF_PPI->CH[PPI_CH_RX_TIMEOUT].EEP = (uint32_t) &RADIO_TIMER->EVENTS_COMPARE[RADIO_TIMER_RX_TIMEOUT_COMPARE];
  NRF_PPI->CH[PPI_CH_RX_TIMEOUT].TEP = (uint32_t) &NRF_RADIO->TASKS_DISABLE;
}

void radio_rx_start() {
  // Enable PPI channels
  NRF_PPI->CHENSET = (1 << PPI_CH_TIMER_START) | (1 << PPI_CH_RX_TIMEOUT);

  RADIO_TIMER->CC[RADIO_TIMER_RX_TIMEOUT_COMPARE] = RX_TIMEOUT_US;

  NRF_RADIO->PACKETPTR = (uint32_t) &m_rx_tx_buf;
  NRF_RADIO->SHORTS    = RADIO_SHORTS_READY_START_Msk | RADIO_SHORTS_END_START_Msk | RADIO_SHORTS_ADDRESS_RSSISTART_Msk;
  clear_events();

  NRF_RADIO->TASKS_RXEN = 1;
}

static void change_to_next_channel() {
  uint8_t next_channel   = channel_resolver_get_next_channel();
  NRF_RADIO->FREQUENCY   = channel_resolver_get_frequency(next_channel);
  NRF_RADIO->DATAWHITEIV = next_channel;
}

static void start_rx_on_next_adv_channel() {
  change_to_next_channel();
  NRF_RADIO->TASKS_RXEN = 1;
}

static void on_rx_packet() {
  if(NRF_RADIO->CRCSTATUS == 1U && (m_rx_tx_buf.s0 & 0x0F) == PACKET_TYPE_ADV_NONCONN_IND) {
    m_on_rx_adv_packet(m_rx_tx_buf, (int)NRF_RADIO->RSSISAMPLE * -1);
  }
}

void RADIO_IRQHandler() {
  NRF_RADIO->EVENTS_READY = 0;

  if(NRF_RADIO->EVENTS_END && (NRF_RADIO->INTENSET & RADIO_INTENSET_END_Msk)) {
    NRF_RADIO->EVENTS_END = 0;
    on_rx_packet();
  } else if(NRF_RADIO->EVENTS_DISABLED && (NRF_RADIO->INTENSET & RADIO_INTENSET_DISABLED_Msk)) {
    NRF_RADIO->EVENTS_DISABLED = 0;
    start_rx_on_next_adv_channel();
  }
}
