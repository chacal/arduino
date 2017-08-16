#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "boards.h"


int main(void) {
  (void) NRF_LOG_INIT(NULL);
  bsp_board_leds_init();

  /* Toggle LEDs. */
  while(true) {
    NRF_LOG_INFO("Blink!\n");
    bsp_board_led_invert(BSP_LED_0);
    nrf_delay_ms(500);
  }
}
