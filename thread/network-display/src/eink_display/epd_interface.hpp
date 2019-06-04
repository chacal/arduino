/**
 *  @filename   :   epdif.h
 *  @brief      :   Header file of epdif.cpp providing EPD interface functions
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

#pragma once

/*
// Pin definition, Waveshare shield
#define RST_PIN         8
#define DC_PIN          6
#define BUSY_PIN        7

#define SPI_SCK_PIN     2
#define SPI_MOSI_PIN    3
#define SPI_MISO_PIN    4
#define SPI_CS_PIN      5
*/

// Pin definition, own custom board
#define RST_PIN        31
#define DC_PIN         29
#define BUSY_PIN       30

#define SPI_SCK_PIN    28
#define SPI_MOSI_PIN    3
#define SPI_MISO_PIN    4  // Not used
#define SPI_CS_PIN      2


class epd_interface {
public:
  epd_interface();

  ~epd_interface();

  static int init();

  static void digital_write(int pin, int value);

  static int digital_read(int pin);

  static void delay_ms(unsigned int delaytime);

  static void spi_transfer(unsigned char data);
};
