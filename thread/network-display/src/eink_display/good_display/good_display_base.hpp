#pragma once

#include <cstdint>
#include <display.hpp>

#define HIGH 1
#define LOW  0

class good_display_base : public display {
public:
  good_display_base(uint16_t w, uint16_t h);

  ~good_display_base() = default;

  void on() override;

protected:

  const uint32_t width;
  const uint32_t height;

  static void send_command(unsigned char command);

  static void send_data(unsigned char data);

  static void reset();

  virtual void set_frame_memory(const unsigned char *image_buffer) = 0;
};
