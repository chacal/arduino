#include <cstdint>
#include <cstdlib>
#include <nrf_log.h>
#include <settings.hpp>
#include "good_display_base.hpp"
#include "il0373_display_base.hpp"
#include "eink_display/epd_interface.hpp"


il0373_display_base::il0373_display_base(uint16_t w, uint16_t h, const il0373_lut &l)
  : good_display_base(w, h), lut{l} {
}

void il0373_display_base::render() {
  init();
  uint32_t expected_size = this->height * this->width * 2 / 8;
  if (this->image_data.size() != expected_size) {
    NRF_LOG_ERROR("Invalid fullscreen bitmap size! Expected %d bytes, got %d bytes.", expected_size, this->image_data.size())
  } else {
    set_frame_memory(this->image_data.data());
    display_frame();
  }
  sleep();
}

void il0373_display_base::clear() {
  this->image_data = std::vector<uint8_t>(this->width * this->height * 2 / 8, 0xFF);
}


void il0373_display_base::draw_fullscreen_bitmap(const std::vector<uint8_t> &vector) {
  this->image_data = vector;
}

int il0373_display_base::init() {
  reset();
  send_command(0x01);     //POWER SETTING
  send_data(0x03);
  send_data(0x00);
  send_data(0x2b);
  send_data(0x2b);
  send_data(0x13);

  send_command(0x06);     //booster soft start
  send_data(0x17);   //A
  send_data(0x17);   //B
  send_data(0x17);   //C

  send_command(0x04);      //Power on
  wait_until_idle();

  send_command(0x00);     //panel setting
  send_data(0x3f);            //KW-3f   KWR-2F  BWROTP 0f BWOTP 1f

  send_command(0x30);     //PLL setting
  send_data(0x3c);            //100hz

  send_command(0x61);     //resolution setting

  send_data(this->height);
  send_data((this->width >> 8) & 0x01);  // Send 9th bit of width
  send_data(this->width & 0xFF);         // Send lowest 8 bits of width

  send_command(0x82);     //vcom_DC setting
  send_data(0x12);

  send_command(0X50);     //VCOM AND DATA INTERVAL SETTING
  send_data(0x97);

  send_lut();

  return 0;
}

void il0373_display_base::send_lut() {
  size_t count;

  send_command(0x20);  //vcom
  for (count = 0; count < sizeof(this->lut.vcom); count++) { send_data(this->lut.vcom[count]); }

  send_command(0x21);  //red not use
  for (count = 0; count < sizeof(this->lut.ww); count++) { send_data(this->lut.ww[count]); }

  send_command(0x22);  //bw r
  for (count = 0; count < sizeof(this->lut.bw); count++) { send_data(this->lut.bw[count]); }

  send_command(0x23);  //wb w
  for (count = 0; count < sizeof(this->lut.wb); count++) { send_data(this->lut.wb[count]); }

  send_command(0x24);  //bb b
  for (count = 0; count < sizeof(this->lut.bb); count++) { send_data(this->lut.bb[count]); }

  send_command(0x25);  //vcom
  for (count = 0; count < sizeof(this->lut.ww); count++) { send_data(this->lut.ww[count]); }
}

void il0373_display_base::set_frame_memory(const unsigned char *image_buffer) {
  send_command(0x10);
  for (uint32_t i = 0; i < width * height / 8; i++) {
    send_data(image_buffer[i]);
  }

  send_command(0x13);
  for (uint32_t i = width * height / 8; i < width * height * 2 / 8; i++) {
    send_data(image_buffer[i]);
  }
}

void il0373_display_base::display_frame() {
  send_command(0x12);     //DISPLAY REFRESH
  epd_interface::delay_ms(1);      //!!!The delay here is necessary, 200uS at least!!!
  wait_until_idle();
}

void il0373_display_base::sleep() {
  send_command(0x50);  //VCOM AND DATA INTERVAL SETTING
  send_data(0xf7);

  send_command(0x02);   //power off
  wait_until_idle();
  send_command(0x07);   //deep sleep
  send_data(0xA5);
}

void il0373_display_base::wait_until_idle() {
  epd_interface::wait_for_pin_state(settings::m_pin_config.epd_busy_pin, HIGH);
}
