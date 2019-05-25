/**
 *  @filename   :   epdif.cpp
 *  @brief      :   Implements EPD interface functions
 *                  Users have to implement all the functions in epdif.cpp
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     August 10 2017
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <nrf_gpio.h>
#include <nrf_delay.h>
#include <nrfx_spim.h>
#include "epd_interface.hpp"

nrfx_spim_t m_spim_instance = NRFX_SPIM_INSTANCE(0);

epd_interface::epd_interface() {
};

epd_interface::~epd_interface() {
};

void epd_interface::digital_write(int pin, int value) {
  if (value > 0) {
    nrf_gpio_pin_set((uint32_t) pin);
  } else {
    nrf_gpio_pin_clear((uint32_t) pin);
  }
}

int epd_interface::digital_read(int pin) {
  return (int) nrf_gpio_pin_read((uint32_t) pin);
}

void epd_interface::delay_ms(unsigned int delaytime) {
  nrf_delay_ms(delaytime);
}

void epd_interface::spi_transfer(unsigned char data) {
  nrfx_spim_xfer_desc_t tx = NRFX_SPIM_XFER_TX(&data, 1);
  APP_ERROR_CHECK(nrfx_spim_xfer(&m_spim_instance, &tx, 0));
}

int epd_interface::init() {
  nrf_gpio_cfg_output(RST_PIN);
  nrf_gpio_cfg_output(DC_PIN);
  nrf_gpio_cfg_input(BUSY_PIN, NRF_GPIO_PIN_PULLDOWN);

  nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
  config.sck_pin   = SPI_SCK_PIN;
  config.mosi_pin  = SPI_MOSI_PIN;
  config.miso_pin  = SPI_MISO_PIN;
  config.ss_pin    = SPI_CS_PIN;
  config.frequency = NRF_SPIM_FREQ_2M;

  APP_ERROR_CHECK(nrfx_spim_init(&m_spim_instance, &config, nullptr, nullptr));
  return 0;
}

