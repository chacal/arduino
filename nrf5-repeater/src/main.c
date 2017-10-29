#include <nrf.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_error.h>
#include <ble_gap.h>
#include <stdlib.h>
#include "util.h"
#include "tx_queue.h"

#define FILTERED_MANUFACTURER_ID   0xDADA
#define TX_DELAY_MIN_US              2000
#define TX_DELAY_MAX_US             50000
#define TX_TIMER               NRF_TIMER1
#define TX_TIMER_CC                     0
#define TX_TIMER_IRQ          TIMER1_IRQn

static inline bool tx_timer_stopped() {
  return TX_TIMER->CC[TX_TIMER_CC] == 0;
}

static inline uint32_t tx_timer_time_left() {
  TX_TIMER->TASKS_CAPTURE[1] = 1;
  return TX_TIMER->CC[TX_TIMER_CC] - TX_TIMER->CC[1];
}

static inline void tx_timer_start_with_delay(uint32_t delay) {
  TX_TIMER->CC[TX_TIMER_CC] = delay;
  TX_TIMER->TASKS_START = 1;
}


static void queue_repeating_tx(nrf_radio_packet_t *adv_packet) {
  uint32_t tx_delay = util_rand_between(TX_DELAY_MIN_US, TX_DELAY_MAX_US);

  if(tx_queue_add_packet(adv_packet, tx_delay) == NRF_SUCCESS) {
    // Start or reconfigure timer if needed
    if(tx_timer_stopped() || tx_timer_time_left() > tx_delay) {
      NRF_LOG_INFO("Start delay: %d\n", tx_delay);
      tx_timer_start_with_delay(tx_delay);
    }
  } else {
    NRF_LOG_ERROR("TX queue full!\n");
  }
}

static void on_rx_adv_packet(nrf_radio_packet_t adv_packet) {
  adv_data_t manuf_data;

  uint32_t res = util_adv_report_parse(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, &adv_packet, &manuf_data);

  if(res == NRF_SUCCESS) {
    uint16_t *manufacturer_id = (uint16_t *) &manuf_data.data[0];  // First two bytes are the manufacturer ID
    if(*manufacturer_id == FILTERED_MANUFACTURER_ID) {
      queue_repeating_tx(&adv_packet);
    }
  }
}

static void tx_timer_init() {
  TX_TIMER->PRESCALER = 4;  // 1MHz, 1 tick == 1µs
  TX_TIMER->BITMODE   = TIMER_BITMODE_BITMODE_16Bit;  // Max timer length == ~65ms
  TX_TIMER->SHORTS    = TIMER_SHORTS_COMPARE0_STOP_Msk | TIMER_SHORTS_COMPARE0_CLEAR_Msk;  // Stop and reset timer on compare0
  TX_TIMER->INTENSET  = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_SetPriority(TX_TIMER_IRQ, 4);
  NVIC_ClearPendingIRQ(TX_TIMER_IRQ);
  NVIC_EnableIRQ(TX_TIMER_IRQ);

  TX_TIMER->CC[TX_TIMER_CC] = 0;
}

static void process_tx_queue() {
  // Disable radio IRQ here
  tx_queue_update_delays(TX_TIMER->CC[TX_TIMER_CC]);

  // Transmit all expired packets
  tx_queue_element_t expired_element;
  while(tx_queue_remove_first_expired_element(&expired_element) == NRF_SUCCESS) {
    NRF_LOG_INFO("TX!\n");
  }

  // Start timer for the shortest existing tx delay
  uint32_t delay_for_next_tx;
  if(tx_queue_get_delay_for_next(&delay_for_next_tx) == NRF_SUCCESS) {
    NRF_LOG_INFO("Next delay: %d\n", delay_for_next_tx);
    tx_timer_start_with_delay(delay_for_next_tx);
  } else {
    TX_TIMER->CC[TX_TIMER_CC] = 0;
  }
  // Enable radio IRQ
}


void TIMER1_IRQHandler() {
  TX_TIMER->EVENTS_COMPARE[TX_TIMER_CC] = 0; // Clear compare event
  process_tx_queue();
}

int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_INFO("Starting..\n");

  util_start_clocks();
  tx_timer_init();
  radio_init(on_rx_adv_packet);
  radio_rx_start();

  NRF_LOG_INFO("Started\n");

  for(;;) {
  }
}
