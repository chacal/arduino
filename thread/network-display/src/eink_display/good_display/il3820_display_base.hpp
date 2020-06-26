#pragma once

#include <cstdint>
#include <memory>
#include "good_display_base.hpp"

extern "C" {
#include <u8g2.h>
}

// IL3820 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF

enum Rotation {
  NO_ROTATION, NINETY_DEG_CW
};


class il3820_display_base : public good_display_base {
public:
  il3820_display_base(uint16_t width, uint16_t h, const uint8_t *lut, Rotation r = NINETY_DEG_CW);

  ~il3820_display_base() = default;

  void render() override;

  void clear() override;

  void draw_fullscreen_bitmap(const std::vector<uint8_t> &) override;

protected:

  const unsigned char *lut;

  u8g2_t u8g2{nullptr};

  std::unique_ptr<uint8_t[]> u8g2_buf = std::make_unique<uint8_t[]>(width * height / 8);

  int init();

  void set_frame_memory(const unsigned char *image_buffer) override;

  static void display_frame();

  static void sleep();

  static void wait_until_idle();

  static void set_memory_area(uint32_t x_start, uint32_t y_start, uint32_t x_end, uint32_t y_end);

  static void set_memory_pointer(uint32_t x, uint32_t y);

  void send_lut();
};
