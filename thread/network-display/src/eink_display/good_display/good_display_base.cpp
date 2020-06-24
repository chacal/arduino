/**
 *  @filename   :   epd2in9.cpp
 *  @brief      :   Implements for e-paper library
 *  @author     :   Yehui from Waveshare
 *
 *  Copyright (C) Waveshare     September 9 2017
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

#include "good_display_base.hpp"
#include "eink_display/epd_interface.hpp"

#define HIGH 1
#define LOW  0

using namespace epd_interface;

good_display_base::~good_display_base() {
}

good_display_base::good_display_base(const uint16_t w, const uint16_t h, const Rotation r) : width{w},
                                                                                             height{h},
                                                                                             rotation{r} {
  epd_interface::init();
}

/**
 *  @brief: basic function for sending commands
 */
void good_display_base::send_command(unsigned char command) {
  digital_write(DC_PIN, LOW);
  spi_transfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void good_display_base::send_data(unsigned char data) {
  digital_write(DC_PIN, HIGH);
  spi_transfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes LOW
 */
void good_display_base::wait_until_idle() {
  wait_for_pin_state(BUSY_PIN, LOW);
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void good_display_base::reset() {
  digital_write(RST_PIN, LOW);                //module reset
  delay_ms(10);
  digital_write(RST_PIN, HIGH);
  delay_ms(10);
}
