#pragma once

#include "good_display_base.hpp"

const unsigned char lut_full_update[] =
                        {
                            0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
                            0x01, 0x00, 0x00, 0x00, 0x00, 0x00
                        };

class good_display_2in13 : public good_display_base {
public:
  good_display_2in13() : good_display_base(128, 250, lut_full_update,
                                           NO_ROTATION) {}  // Display is actually 122 * 250, but logically 128 * 250

  void set_frame_memory(const unsigned char *image_buffer, uint32_t x, uint32_t y, uint32_t image_width, uint32_t image_height) override {
    uint32_t x_end;
    uint32_t y_end;

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
        ) {
      return;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= this->width) {
      x_end = this->width - 1;
    } else {
      x_end = x + image_width - 1;
    }
    if (y + image_height >= this->height) {
      y_end = this->height - 1;
    } else {
      y_end = y + image_height - 1;
    }

    set_memory_area(x, y, x_end, y_end);
    /* set the frame memory line by line */
    for (int j = y; j <= y_end; j++) {
      set_memory_pointer(x, j);
      send_command(WRITE_RAM);
      for (int i = x / 8; i <= x_end / 8; i++) {
        send_data(image_buffer[(i - x / 8) + (j - y) * (image_width / 8)]);
      }
    }
  }


  void set_frame_memory(const unsigned char *image_buffer) override {
    set_memory_area(0, 0, this->width - 1, this->height - 1);
    /* set the frame memory line by line */
    for (int j = 0; j < this->height; j++) {
      set_memory_pointer(0, j);
      send_command(WRITE_RAM);
      for (int i = 0; i < this->width / 8; i++) {
        send_data(image_buffer[i + j * (this->width / 8)]);
      }
    }
  }

  void clear_frame_memory(unsigned char color) override {
    set_memory_area(0, 0, this->width - 1, this->height - 1);
    /* set the frame memory line by line */
    for (int j = 0; j < this->height; j++) {
      set_memory_pointer(0, j);
      send_command(WRITE_RAM);
      for (int i = 0; i < this->width / 8; i++) {
        send_data(color);
      }
    }
  }

};