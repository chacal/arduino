#include <cstdint>
#include "good_display_base.hpp"
#include "il3820_display_base.hpp"
#include "eink_display/epd_interface.hpp"


using namespace epd_interface;

il3820_display_base::~il3820_display_base() {
}

il3820_display_base::il3820_display_base(const uint16_t w, const uint16_t h, const uint8_t *lut, const Rotation r)
  : good_display_base(w, h, r) {
  epd_interface::init();
  this->lut = lut;
}

int il3820_display_base::init() {
  /* EPD hardware init start */
  reset();
  send_command(DRIVER_OUTPUT_CONTROL);
  send_data((this->height - 1) & 0xFF);
  send_data(((this->height - 1) >> 8) & 0xFF);
  send_data(0x00);                     // GD = 0; SM = 0; TB = 0;
  send_command(BOOSTER_SOFT_START_CONTROL);
  send_data(0xD7);
  send_data(0xD6);
  send_data(0x9D);
  send_command(WRITE_VCOM_REGISTER);
  send_data(0xA8);                     // VCOM 7C
  send_command(SET_DUMMY_LINE_PERIOD);
  send_data(0x1A);                     // 4 dummy lines per gate
  send_command(SET_GATE_TIME);
  send_data(0x08);                     // 2us per line
  send_command(DATA_ENTRY_MODE_SETTING);
  send_data(0x03);                     // X increment; Y increment
  set_lut(this->lut);
  /* EPD hardware init end */
  return 0;
}

/**
 *  @brief: set the look-up table register
 */
void il3820_display_base::set_lut(const unsigned char *lut) {
  this->lut = lut;
  send_command(WRITE_LUT_REGISTER);
  /* the length of look-up table is 30 bytes */
  for (int i = 0; i < 30; i++) {
    send_data(this->lut[i]);
  }
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 */
void il3820_display_base::set_frame_memory(
  const unsigned char *image_buffer,
  uint32_t x,
  uint32_t y,
  uint32_t image_width,
  uint32_t image_height
) {
  uint32_t x_end;
  uint32_t y_end;

  if (
    image_buffer == nullptr ||
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
  set_memory_pointer(x, y);
  send_command(WRITE_RAM);
  /* send the image data */
  for (uint32_t j = 0; j < y_end - y + 1; j++) {
    for (uint32_t i = 0; i < (x_end - x + 1) / 8; i++) {
      send_data(image_buffer[i + j * (image_width / 8)]);
    }
  }
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 *
 *          Question: When do you use this function instead of 
 *          void SetFrameMemory(
 *              const unsigned char* image_buffer,
 *              int x,
 *              int y,
 *              int image_width,
 *              int image_height
 *          );
 *          Answer: SetFrameMemory with parameters only reads image data
 *          from the RAM but not from the flash in AVR chips (for AVR chips,
 *          you have to use the function pgm_read_byte to read buffers 
 *          from the flash).
 */
void il3820_display_base::set_frame_memory(const unsigned char *image_buffer) {
  set_memory_area(0, 0, this->width - 1, this->height - 1);
  set_memory_pointer(0, 0);
  send_command(WRITE_RAM);
  /* send the image data */
  for (uint32_t i = 0; i < this->width / 8 * this->height; i++) {
    send_data(image_buffer[i]);
  }
}

/**
 *  @brief: clear the frame memory with the specified color.
 *          this won't update the display.
 */
void il3820_display_base::clear_frame_memory(unsigned char color) {
  set_memory_area(0, 0, this->width - 1, this->height - 1);
  set_memory_pointer(0, 0);
  send_command(WRITE_RAM);
  /* send the color data */
  for (uint32_t i = 0; i < this->width / 8 * this->height; i++) {
    send_data(color);
  }
}

/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will 
 *          set the other memory area.
 */
void il3820_display_base::display_frame() {
  send_command(DISPLAY_UPDATE_CONTROL_2);
  send_data(0xC4);
  send_command(MASTER_ACTIVATION);
  send_command(TERMINATE_FRAME_READ_WRITE);
  wait_until_idle();
}

/**
 *  @brief: private function to specify the memory area for data R/W
 */
void il3820_display_base::set_memory_area(uint32_t x_start, uint32_t y_start, uint32_t x_end, uint32_t y_end) {
  send_command(SET_RAM_X_ADDRESS_START_END_POSITION);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  send_data((x_start >> 3) & 0xFF);
  send_data((x_end >> 3) & 0xFF);
  send_command(SET_RAM_Y_ADDRESS_START_END_POSITION);
  send_data(y_start & 0xFF);
  send_data((y_start >> 8) & 0xFF);
  send_data(y_end & 0xFF);
  send_data((y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
void il3820_display_base::set_memory_pointer(uint32_t x, uint32_t y) {
  send_command(SET_RAM_X_ADDRESS_COUNTER);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  send_data((x >> 3) & 0xFF);
  send_command(SET_RAM_Y_ADDRESS_COUNTER);
  send_data(y & 0xFF);
  send_data((y >> 8) & 0xFF);
  wait_until_idle();
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          You can use Epd::Init() to awaken
 */
void il3820_display_base::sleep() {
  send_command(DEEP_SLEEP_MODE);
  send_data(0x01);
}
