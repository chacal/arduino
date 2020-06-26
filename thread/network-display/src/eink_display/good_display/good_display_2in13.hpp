#pragma once

#include "il3820_display_base.hpp"

const unsigned char lut_2in13[] =
                      {
                        0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
                        0x01, 0x00, 0x00, 0x00, 0x00, 0x00
                      };

class good_display_2in13 : public il3820_display_base {
public:
  good_display_2in13() : il3820_display_base(128, 250, lut_2in13,
                                             NO_ROTATION) {}  // Display is actually 122 * 250, but logically 128 * 250


  void set_frame_memory(const unsigned char *image_buffer) override {
    set_memory_area(0, 0, this->width - 1, this->height - 1);
    /* set the frame memory line by line */
    for (uint32_t j = 0; j < this->height; j++) {
      set_memory_pointer(0, j);
      send_command(WRITE_RAM);
      for (uint32_t i = 0; i < this->width / 8; i++) {
        send_data(image_buffer[i + j * (this->width / 8)]);
      }
    }
  }
};