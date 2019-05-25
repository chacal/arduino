#include "nokia_display.hpp"

#define CONTRAST       130

nokia_display::nokia_display() {
  u8g2_Setup_pcd8544_84x48_f(&u8g2, U8G2_R0, adapter.u8x8_byte_nrf52_hw_spi, adapter.u8x8_gpio_and_delay_nrf52);
  u8g2_InitDisplay(&u8g2);
  u8g2_SetContrast(&u8g2, CONTRAST);
  u8g2_SetFont(&u8g2, u8g2_font_helvB10_tr);
  u8g2_ClearDisplay(&u8g2);
}

void nokia_display::on() {
  clear();
  render();
  u8g2_SetPowerSave(&u8g2, 0);
}

void nokia_display::clear() {
  u8g2_ClearBuffer(&u8g2);
}

void nokia_display::render() {
  u8g2_SendBuffer(&u8g2);
}
