#include "display.h"
#include "power_manager.h"
#include <u8g2.h>
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrf_drv_spi.h>

#define SCK_PIN         11   // Display PIN 1
#define MOSI_PIN        12   // Display PIN 2
#define DC_PIN          13   // Display PIN 4
#define RESET_PIN        6   // Display PIN 5
#define CS_PIN           7   // Display PIN 6

#define CONTRAST       230

#define SPI_INSTANCE     0
#define SCREEN_WIDTH   128

static u8g2_t u8g2;

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
static volatile bool       spi_xfer_done;


static uint8_t u8x8_gpio_and_delay_nrf52(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
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


static void spi_evt_handler(nrf_drv_spi_evt_t const *p_event, void *p_context) {
  spi_xfer_done = true;
}

static void spi_init() {
  nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
  spi_config.ss_pin   = CS_PIN;
  spi_config.miso_pin = NRF_DRV_SPI_PIN_NOT_USED;
  spi_config.mosi_pin = MOSI_PIN;
  spi_config.sck_pin  = SCK_PIN;
  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, spi_evt_handler, NULL));
}

static void spi_send(uint8_t *data, uint8_t length) {
  spi_xfer_done = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, data, length, NULL, 0));
  while(!spi_xfer_done) {
    power_manage();
  }
}

static uint8_t u8x8_byte_nrf52_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
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


void display_init() {
  u8g2_Setup_st7565_nhd_c12864_f(&u8g2, U8G2_R0, u8x8_byte_nrf52_hw_spi, u8x8_gpio_and_delay_nrf52);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetContrast(&u8g2, CONTRAST);
  u8g2_SetFont(&u8g2, u8g2_font_helvB10_tr);
  u8g2_ClearDisplay(&u8g2);
}

void display_on() {
  display_clear();
  display_render();
  u8g2_SetPowerSave(&u8g2, 0);
}

void display_off() {
  u8g2_SetPowerSave(&u8g2, 1);
}

static const uint8_t *font_for_size(uint8_t font_size) {
  if(font_size <= 8) {
    return u8g2_font_helvB08_tr;
  } else if(font_size <= 10) {
    return u8g2_font_helvB10_tr;
  } else if(font_size <= 12) {
    return u8g2_font_helvB12_tr;
  } else if(font_size <= 14) {
    return u8g2_font_helvB14_tr;
  } else if(font_size <= 18) {
    return u8g2_font_helvB18_tr;
  } else if(font_size <= 24) {
    return u8g2_font_helvB24_tr;
  } else if(font_size <= 25) {
    return u8g2_font_fub25_tn;
  } else if(font_size <= 30) {
    return u8g2_font_fub30_tn;
  } else if(font_size <= 35) {
    return u8g2_font_fub35_tn;
  } else {
    return u8g2_font_fub42_tn;
  }
}

void display_draw_str(uint8_t x, uint8_t y, uint8_t font_size, char *str) {
  const uint8_t *font = font_for_size(font_size);
  u8g2_SetFont(&u8g2, font);
  u8g2_DrawStr(&u8g2, x, y, str);
}

void display_clear() {
  u8g2_ClearBuffer(&u8g2);
}

void display_draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y) {
  u8g2_DrawLine(&u8g2, start_x, start_y, end_x, end_y);
}

void display_render() {
  u8g2_SendBuffer(&u8g2);
}

uint8_t display_centered_x(const char *str, uint8_t font_size) {
  u8g2_SetFont(&u8g2, font_for_size(font_size));
  int x = SCREEN_WIDTH / 2 - u8g2_GetStrWidth(&u8g2, str) / 2;
  return x >= 0 ? x : 0;
}
