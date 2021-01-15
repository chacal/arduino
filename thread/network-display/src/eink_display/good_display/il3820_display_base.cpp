#include <cstdint>
#include <cmath>
#include <nrf_log.h>
#include <settings.hpp>
#include "good_display_base.hpp"
#include "il3820_display_base.hpp"
#include "eink_display/epd_interface.hpp"


u8x8_display_info_t m_display_info;

uint8_t in_memory_display_handler(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  if (msg == U8X8_MSG_DISPLAY_SETUP_MEMORY) {
    u8x8_d_helper_display_setup_memory(u8x8, &m_display_info);
  }
  return 0;
}

uint8_t u8x8_gpio_and_delay_dummy(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  u8x8_SetGPIOResult(u8x8, 1);      // default return value
  return 1;
}

il3820_display_base::il3820_display_base(const uint16_t w, const uint16_t h, const uint8_t *l, const Rotation r)
  : good_display_base(w, h), lut{l} {

  m_display_info = [&] {
    u8x8_display_info_t di{};
    di.tile_width   = ceil(w / 8.0);
    di.tile_height  = ceil(h / 8.0);
    di.pixel_width  = w;
    di.pixel_height = h;
    return di;
  }();

  auto u8g2_rotation = r == NO_ROTATION ? U8G2_R0 : U8G2_R1;
  u8g2_SetupDisplay(&u8g2, in_memory_display_handler, u8x8_cad_011, u8x8_byte_empty, u8x8_gpio_and_delay_dummy);
  u8g2_SetupBuffer(&u8g2, u8g2_buf.get(), m_display_info.tile_height, u8g2_ll_hvline_horizontal_right_lsb, u8g2_rotation);
}

void il3820_display_base::render() {
  init();

  uint8_t       tmp_buf[width * height / 8];
  // u8g2 has 0 as white and 1 as black, display vice versa
  // -> invert all bits before writing to display memory
  for (uint32_t i = 0; i < width * height / 8; ++i) {
    tmp_buf[i] = ~u8g2_buf[i];
  }
  set_frame_memory(tmp_buf);
  display_frame();
  sleep();
}

void il3820_display_base::draw_fullscreen_bitmap(const std::vector<uint8_t> &bitmap) {
  uint32_t expected_size = u8g2.height * u8g2.width / 8;
  if (bitmap.size() != expected_size) {
    NRF_LOG_ERROR("Invalid fullscreen bitmap size! Expected %d bytes, got %d bytes.", expected_size, bitmap.size())
  } else {
    u8g2_DrawBitmap(&u8g2, 0, 0, u8g2.width / 8, u8g2.height, bitmap.data());
  }
}

void il3820_display_base::clear() {
  u8g2_ClearBuffer(&u8g2);
}

int il3820_display_base::init() {
  if (settings::m_pin_config.epd_reset_ctl1_pin > 0) {
    epd_interface::digital_write(settings::m_pin_config.epd_reset_ctl1_pin, HIGH);
  }
  /* EPD hardware init start */
  reset();
  send_command(DRIVER_OUTPUT_CONTROL);
  send_data((height - 1) & 0xFF);
  send_data(((height - 1) >> 8) & 0xFF);
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
  send_lut();
  /* EPD hardware init end */
  return 0;
}

void il3820_display_base::send_lut() {
  send_command(WRITE_LUT_REGISTER);
  /* the length of look-up table is 30 bytes */
  for (int i = 0; i < 30; i++) {
    send_data(this->lut[i]);
  }
}

void il3820_display_base::set_frame_memory(const unsigned char *image_buffer) {
  set_memory_area(0, 0, this->width - 1, this->height - 1);
  set_memory_pointer(0, 0);
  send_command(WRITE_RAM);
  /* send the image data */
  for (uint32_t i = 0; i < this->width / 8 * this->height; i++) {
    send_data(image_buffer[i]);
  }
}

void il3820_display_base::display_frame() {
  send_command(DISPLAY_UPDATE_CONTROL_2);
  send_data(0xC4);
  send_command(MASTER_ACTIVATION);
  send_command(TERMINATE_FRAME_READ_WRITE);
  wait_until_idle();
}

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

void il3820_display_base::set_memory_pointer(uint32_t x, uint32_t y) {
  send_command(SET_RAM_X_ADDRESS_COUNTER);
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  send_data((x >> 3) & 0xFF);
  send_command(SET_RAM_Y_ADDRESS_COUNTER);
  send_data(y & 0xFF);
  send_data((y >> 8) & 0xFF);
  wait_until_idle();
}

void il3820_display_base::sleep() {
  send_command(DEEP_SLEEP_MODE);
  send_data(0x01);
  if (settings::m_pin_config.epd_reset_ctl1_pin > 0) {
    epd_interface::digital_write(settings::m_pin_config.epd_reset_ctl1_pin, LOW);
  }
}

void il3820_display_base::wait_until_idle() {
  epd_interface::wait_for_pin_state(settings::m_pin_config.epd_busy_pin, LOW);
}