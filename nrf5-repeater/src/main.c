#include <nrf.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_error.h>
#include <ble_gap.h>
#include <stdlib.h>
#include "util.h"
#include "tx_queue.h"
#include "tx_timer.h"

#define FILTERED_MANUFACTURER_ID   0xDADA
#define TX_DELAY_MIN_US              2000
#define TX_DELAY_MAX_US             50000


static void queue_repeating_tx(nrf_radio_packet_t *adv_packet) {
  uint32_t tx_delay = util_rand_between(TX_DELAY_MIN_US, TX_DELAY_MAX_US);

  if(tx_queue_add_packet(adv_packet, tx_delay) == NRF_SUCCESS) {
    // Start or reconfigure timer if needed
    if(tx_timer_stopped() || tx_timer_time_left() > tx_delay) {
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
      NRF_LOG_INFO("RX\n");
      queue_repeating_tx(&adv_packet);
    }
  }
}

static void process_tx_queue() {
  radio_disable_irq();

  tx_queue_update_delays(tx_timer_get_current_delay());

  // Transmit all expired packets
  tx_queue_element_t expired_element;
  while(tx_queue_remove_first_expired_element(&expired_element) == NRF_SUCCESS) {
    NRF_LOG_INFO("TX\n");
    radio_send_packet(&expired_element.tx_packet);
  }

  // Start timer for the shortest existing tx delay or disable if no packets in queue
  uint32_t delay_for_next_tx;
  if(tx_queue_get_delay_for_next(&delay_for_next_tx) == NRF_SUCCESS) {
    tx_timer_start_with_delay(delay_for_next_tx);
  } else {
    tx_timer_set_delay(0);
  }

  radio_rx_start();

  radio_enable_irq();
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_INFO("Starting..\n");

  util_init_rand();
  util_start_clocks();
  tx_timer_init(process_tx_queue);
  radio_init(on_rx_adv_packet);
  radio_rx_start();

  NRF_LOG_INFO("Started\n");

  for(;;) {
  }
}
