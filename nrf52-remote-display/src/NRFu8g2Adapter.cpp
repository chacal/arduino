#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf_drv_spi.h>

#include "NRFu8g2Adapter.hpp"
#include "PowerManager.hpp"

#define SCK_PIN         11   // Display PIN 1
#define MOSI_PIN        12   // Display PIN 2
#define DC_PIN          13   // Display PIN 4
#define RESET_PIN        6   // Display PIN 5
#define CS_PIN           7   // Display PIN 6

#define SPI_INSTANCE     0

NRFu8g2Adapter &NRFu8g2Adapter::getInstance() {
  static NRFu8g2Adapter theInstance;
  return theInstance;
}

nrf_drv_spi_t NRFu8g2Adapter::spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
volatile bool NRFu8g2Adapter::spi_xfer_done = false;

NRFu8g2Adapter::NRFu8g2Adapter() = default;

void NRFu8g2Adapter::spi_evt_handler(nrf_drv_spi_evt_t const *p_event, void *p_context) {
  spi_xfer_done = true;
}

void NRFu8g2Adapter::spi_init() {
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.ss_pin   = CS_PIN;
  spi_config.miso_pin = NRF_DRV_SPI_PIN_NOT_USED;
  spi_config.mosi_pin = MOSI_PIN;
  spi_config.sck_pin  = SCK_PIN;
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_evt_handler, NULL));
}

void NRFu8g2Adapter::spi_send(uint8_t *data, uint8_t length) {
  spi_xfer_done = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, data, length, NULL, 0));
  while(!spi_xfer_done) {
    PowerManager::getInstance().manage();
  }
}

uint8_t NRFu8g2Adapter::u8x8_gpio_and_delay_nrf52(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  switch(msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:  // called once during init phase of u8g2/u8x8, can be used to setup pins
      nrf_gpio_cfg(SCK_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
      nrf_gpio_cfg(MOSI_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
      nrf_gpio_cfg_output(CS_PIN);
      nrf_gpio_cfg_output(DC_PIN);
      nrf_gpio_cfg_output(RESET_PIN);
      break;
    case U8X8_MSG_DELAY_NANO:      // delay arg_int * 1 nano second
      break;
    case U8X8_MSG_DELAY_100NANO:    // delay arg_int * 100 nano seconds
      break;
    case U8X8_MSG_DELAY_10MICRO:    // delay arg_int * 10 micro seconds
      nrf_delay_us(arg_int * 10u);
      break;
    case U8X8_MSG_DELAY_MILLI:      // delay arg_int * 1 milli second
      nrf_delay_ms(arg_int);
      break;
    case U8X8_MSG_GPIO_SPI_CLOCK:   // D0 or SPI clock pin: Output level in arg_int
      nrf_gpio_pin_write(SCK_PIN, arg_int);
      break;
    case U8X8_MSG_GPIO_SPI_DATA:    // D1 or SPI data pin: Output level in arg_int
      nrf_gpio_pin_write(MOSI_PIN, arg_int);
      break;
    case U8X8_MSG_GPIO_CS:        // CS (chip select) pin: Output level in arg_int
      nrf_gpio_pin_write(CS_PIN, arg_int);
      break;
    case U8X8_MSG_GPIO_DC:        // DC (data/cmd, A0, register select) pin: Output level in arg_int
      nrf_gpio_pin_write(DC_PIN, arg_int);
      break;
    case U8X8_MSG_GPIO_RESET:      // Reset pin: Output level in arg_int
      nrf_gpio_pin_write(RESET_PIN, arg_int);
      break;
    default:
      u8x8_SetGPIOResult(u8x8, 1);      // default return value
      break;
  }
  return 1;
}

uint8_t NRFu8g2Adapter::u8x8_byte_nrf52_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  switch(msg) {
    case U8X8_MSG_BYTE_INIT:
      spi_init();
      break;
    case U8X8_MSG_BYTE_SET_DC:
      u8x8_gpio_SetDC(u8x8, arg_int);
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      break;
    case U8X8_MSG_BYTE_SEND:
      spi_send((uint8_t *) arg_ptr, arg_int);
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      break;
    default:
      return 0;
  }
  return 1;
}
