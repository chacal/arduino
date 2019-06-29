
#include <app_error.h>
#include "eink_display.hpp"

u8x8_display_info_t m_display_info;

uint8_t in_memory_display_handler(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  switch (msg) {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
      u8x8_d_helper_display_setup_memory(u8x8, &m_display_info);
    default:;
  }
  return 0;
}

uint8_t u8x8_gpio_and_delay_dummy(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
  u8x8_SetGPIOResult(u8x8, 1);      // default return value
  return 1;
}

eink_display::eink_display() {
  m_display_info = [&] {
    u8x8_display_info_t di{};
    di.tile_width   = epd.width / 8;
    di.tile_height  = epd.height / 8;
    di.pixel_width  = epd.width;
    di.pixel_height = epd.height;
    return di;
  }();

  u8g2_SetupDisplay(&u8g2, in_memory_display_handler, u8x8_cad_011, u8x8_byte_empty, u8x8_gpio_and_delay_dummy);
  u8g2_SetupBuffer(&u8g2, u8g2_buf.get(), m_display_info.tile_height, u8g2_ll_hvline_horizontal_right_lsb, U8G2_R1);
}

void eink_display::on() {
  clear();
  // Render twice to remove any potential ghosting of the old image
  render();
  render();
}

void eink_display::render() {
  // Wake up and initialize display
  APP_ERROR_CHECK(epd.init());

  // u8g2 has 0 as white and 1 as black, display vice versa
  // -> invert all bits before writing to display memory
  for (uint32_t i = 0; i < epd.width * epd.height / 8; ++i) {
    epd_buf[i] = ~u8g2_buf[i];
  }

  epd.set_frame_memory(epd_buf.get());
  epd.display_frame();
  epd.sleep();
}

void eink_display::clear() {
  u8g2_ClearBuffer(&u8g2);
}
