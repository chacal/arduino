#pragma once

#include <nrf_spi.h>
#include <nrf_drv_spi.h>

extern "C" {
#include <u8g2.h>
}

class NRFu8g2Adapter {
  static nrf_drv_spi_t spi;
  static volatile bool spi_xfer_done;

  NRFu8g2Adapter();
  static void spi_evt_handler(nrf_drv_spi_evt_t const *p_event, void *p_context);
  static void spi_init();
  static void spi_send(uint8_t *data, uint8_t length);

public:
  static NRFu8g2Adapter &getInstance();

  static uint8_t u8x8_gpio_and_delay_nrf52(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
  static uint8_t u8x8_byte_nrf52_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
};
