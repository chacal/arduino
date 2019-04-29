#include <app_error.h>
#include "EInkDisplay.hpp"

static u8g2_t  m_u8g2;
static Epd     m_epd = Epd{};
static uint8_t m_u8g2_buf[EPD_WIDTH * EPD_HEIGHT / 8];
static uint8_t m_epd_buf[EPD_WIDTH * EPD_HEIGHT / 8];

constexpr auto m_display_info = [] {
  u8x8_display_info_t di{};
  di.tile_width   = EPD_WIDTH / 8;  // 16 * 8 => 128 pixels
  di.tile_height  = EPD_HEIGHT / 8;  // 37 * 8 => 296 pixels
  di.pixel_width  = EPD_WIDTH;
  di.pixel_height = EPD_HEIGHT;
  return di;
}();


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


namespace EInkDisplay {

  void init() {
    APP_ERROR_CHECK(m_epd.Init(lut_full_update));
    u8g2_SetupDisplay(&m_u8g2, in_memory_display_handler, u8x8_cad_011, u8x8_byte_empty, u8x8_gpio_and_delay_dummy);
    u8g2_SetupBuffer(&m_u8g2, m_u8g2_buf, m_display_info.tile_height, u8g2_ll_hvline_horizontal_right_lsb, U8G2_R1);
  }

  u8g2_t &getU8g2() {
    return m_u8g2;
  }

  void render() {
    // u8g2 has 0 as white and 1 as black, display vice versa
    // -> invert all bits before writing to display memory
    for (uint32_t i = 0; i < sizeof(m_u8g2_buf); ++i) {
      m_epd_buf[i] = ~m_u8g2_buf[i];
    }

    m_epd.SetFrameMemory(m_epd_buf);
    m_epd.DisplayFrame();
  }

  void sleep() {
    m_epd.Sleep();
  }
}