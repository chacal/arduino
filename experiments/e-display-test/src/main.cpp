#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>
#include <epd2in9/epd2in9.h>
#include "EInkDisplay.hpp"

extern "C" {
#include <nrf_pwr_mgmt.h>
}

static void log_init() {
  ret_code_t err_code = NRF_LOG_INIT(nullptr);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void power_management_init() {
  ret_code_t err_code;
  err_code = nrf_pwr_mgmt_init();
  APP_ERROR_CHECK(err_code);
}

int main(int argc, char *argv[]) {
  log_init();
  power_management_init();
  EInkDisplay::init();

  NRF_LOG_INFO("E-Display test started");

  auto u8g2 = EInkDisplay::getU8g2();
  u8g2_DrawCircle(&u8g2, 30, 40, 20, U8G2_DRAW_ALL);
  u8g2_DrawCircle(&u8g2, 30, 100, 20, U8G2_DRAW_ALL);

  u8g2_SetFont(&u8g2, u8g2_font_inr16_mr);
  u8g2_DrawStr(&u8g2, 80, 127, "Hello World!");

  EInkDisplay::render();
  EInkDisplay::sleep();

  NRF_LOG_INFO("All done!");

  for (;;) {
    nrf_pwr_mgmt_run();
  }
}
