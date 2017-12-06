#include <nrf.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <app_error.h>
#include <ble_gap.h>
#include <stdlib.h>
#include "util.h"

#define FILTERED_MANUFACTURER_ID   0xDADA


static void on_rx_adv_packet(nrf_radio_packet_t adv_packet) {
  adv_data_t manuf_data;

  uint32_t res = util_adv_report_parse(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, &adv_packet, &manuf_data);

  if(res == NRF_SUCCESS) {
    uint16_t *manufacturer_id = (uint16_t *) &manuf_data.data[0];  // First two bytes are the manufacturer ID
    if(*manufacturer_id == FILTERED_MANUFACTURER_ID) {
      NRF_LOG_INFO("RX\n");
      // TODO: Write packet to UART here
    }
  }
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_INFO("Starting..\n");

  util_start_clocks();
  radio_init(on_rx_adv_packet);
  radio_rx_start();

  NRF_LOG_INFO("Started\n");

  for(;;) {
  }
}
