#pragma once

#include <cstdint>
#include <display.hpp>
#include <good_display_base.hpp>

struct il0373_lut {
  uint8_t vcom[44];
  uint8_t ww[42];
  uint8_t bw[42];
  uint8_t wb[42];
  uint8_t bb[42];
};


class il0373_display_base : public good_display_base {
public:
  il0373_display_base(uint16_t width, uint16_t h, const il0373_lut &lut);

  ~il0373_display_base() = default;

  void render() override;

  void clear() override;

  void draw_fullscreen_bitmap(const std::vector<uint8_t> &vector) override;

protected:

  const il0373_lut lut;

  std::vector<uint8_t> image_data = std::vector<uint8_t>(this->width * this->height * 2 / 8);

  int init();

  void set_frame_memory(const unsigned char *image_buffer) override;

  void send_lut();

  static void display_frame();

  static void sleep();

  static void wait_until_idle();
};
