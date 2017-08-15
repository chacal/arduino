#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#define LED_PIN 3


int main(void) {
  (void) NRF_LOG_INIT(NULL);

  nrf_gpio_cfg_output(LED_PIN);
  nrf_gpio_pin_write(LED_PIN, 0);

  /* Toggle LEDs. */
  while(true) {
    NRF_LOG_INFO("Blink!\n");
    nrf_gpio_pin_toggle(LED_PIN);
    nrf_delay_ms(2000);
  }
}

