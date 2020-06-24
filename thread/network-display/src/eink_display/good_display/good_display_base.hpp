/**
 *  @filename   :   epd2in9.h
 *  @brief      :   Header file for e-paper display library epd2in9.cpp
 *  @author     :   Yehui from Waveshare
 *  
 *  Copyright (C) Waveshare     September 5 2017
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

#include <cstdint>

enum Rotation {
  NO_ROTATION, NINETY_DEG_CW
};

class good_display_base {
public:
  good_display_base(uint16_t w, uint16_t h, Rotation r = NINETY_DEG_CW);

  ~good_display_base();

  const uint32_t width;
  const uint32_t height;
  const Rotation rotation;

  void send_command(unsigned char command);

  void send_data(unsigned char data);

  void wait_until_idle();

  void reset();

  virtual int init() = 0;

  virtual void set_frame_memory(const unsigned char *image_buffer,
                                uint32_t x,
                                uint32_t y,
                                uint32_t image_width,
                                uint32_t image_height) = 0;

  virtual void set_frame_memory(const unsigned char *image_buffer) = 0;

  virtual void clear_frame_memory(unsigned char color) = 0;

  virtual void display_frame() = 0;

  virtual void sleep() = 0;
};
