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
#include <nrfx_spim.h>
#include <nrfx_rtc.h>
#include <app_timer.h>
#include <nrfx_gpiote.h>
#include <settings.hpp>
#include "epd_interface.hpp"

bool        m_initialized = false;
nrfx_spim_t m_spim_instance;
nrfx_rtc_t  m_rtc;
bool        m_sleeping;

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

void rtc_handler(nrfx_rtc_int_type_t int_type) {
  m_sleeping = false;
}

void epd_interface::delay_ms(unsigned int delaytime) {
  auto delay_ticks = APP_TIMER_TICKS(delaytime);

  //Set compare channel to trigger interrupt after delay_ticks
  APP_ERROR_CHECK(nrfx_rtc_cc_set(&m_rtc, 0, delay_ticks, true));
  m_sleeping = true;

  nrfx_rtc_enable(&m_rtc);

  while (m_sleeping) {
    __SEV();
    __WFE();
    __WFE();
  }

  nrfx_rtc_disable(&m_rtc);
  nrfx_rtc_counter_clear(&m_rtc);
}

void epd_interface::spi_transfer(unsigned char data) {
  nrfx_spim_xfer_desc_t tx = NRFX_SPIM_XFER_TX(&data, 1);
  APP_ERROR_CHECK(nrfx_spim_xfer(&m_spim_instance, &tx, 0));
}

void epd_interface::wait_for_pin_state(uint8_t pin, uint8_t state) {
  nrfx_gpiote_in_config_t pin_config;
  if (state > 0) {
    pin_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);
  } else {
    pin_config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
  }
  APP_ERROR_CHECK(nrfx_gpiote_in_init(pin, &pin_config, nullptr));

  if (digital_read(pin) != state) {
    nrfx_gpiote_in_event_enable(pin, true);
    while (digital_read(pin) != state) {
      __SEV();
      __WFE();
      __WFE();
    }
    nrfx_gpiote_in_event_disable(pin);
  }

  nrfx_gpiote_in_uninit(pin);
}

int initialize_rtc() {
  m_rtc.p_reg            = NRF_RTC0;
  m_rtc.irq              = RTC0_IRQn;
  m_rtc.instance_id      = NRFX_RTC0_INST_IDX;
  m_rtc.cc_channel_count = RTC0_CC_NUM;

  //Initialize RTC instance
  nrfx_rtc_config_t config = {0};
  config.prescaler          = RTC_FREQ_TO_PRESCALER(NRFX_RTC_DEFAULT_CONFIG_FREQUENCY);
  config.interrupt_priority = NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY;
  config.tick_latency       = NRFX_RTC_US_TO_TICKS(NRFX_RTC_MAXIMUM_LATENCY_US, NRFX_RTC_DEFAULT_CONFIG_FREQUENCY);
  config.reliable           = NRFX_RTC_DEFAULT_CONFIG_RELIABLE;
  return nrfx_rtc_init(&m_rtc, &config, rtc_handler);
}

int initialize_spi() {
  m_spim_instance = NRFX_SPIM_INSTANCE(0);

  auto               pins   = settings::m_pin_config;
  nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
  config.sck_pin   = pins.spi_clk_pin;
  config.mosi_pin  = pins.spi_mosi_pin;
  config.miso_pin  = pins.spi_miso_pin;
  config.ss_pin    = pins.epd_cs_pin;
  config.frequency = NRF_SPIM_FREQ_2M;

  return nrfx_spim_init(&m_spim_instance, &config, nullptr, nullptr);
}

int epd_interface::init() {
  if (!m_initialized) {
    m_sleeping = false;

    APP_ERROR_CHECK(initialize_rtc());
    APP_ERROR_CHECK(initialize_spi());
    if (!nrfx_gpiote_is_init()) {
      APP_ERROR_CHECK(nrfx_gpiote_init());
    }

    auto pins = settings::m_pin_config;
    nrf_gpio_cfg_output(pins.epd_rst_pin);
    nrf_gpio_cfg_output(pins.epd_dc_pin);
    if (pins.epd_reset_ctl1_pin > 0) {
      nrf_gpio_cfg_output(pins.epd_reset_ctl1_pin);
    }
    if (pins.epd_reset_ctl2_pin > 0) {
      nrf_gpio_cfg_output(pins.epd_reset_ctl2_pin);
    }

    m_initialized = true;
  }
  return 0;
}

