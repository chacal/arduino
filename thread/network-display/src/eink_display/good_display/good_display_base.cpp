#include <settings.hpp>
#include "good_display_base.hpp"
#include "eink_display/epd_interface.hpp"

using namespace epd_interface;

good_display_base::good_display_base(const uint16_t w, const uint16_t h) : width{w},
                                                                           height{h} {
  epd_interface::init();
}

void good_display_base::on() {
  clear();
  // Render twice to remove any potential ghosting of the old image
  render();
  render();
}


void good_display_base::send_command(unsigned char command) {
  digital_write(settings::m_pin_config.epd_dc_pin, LOW);
  spi_transfer(command);
}

void good_display_base::send_data(unsigned char data) {
  digital_write(settings::m_pin_config.epd_dc_pin, HIGH);
  spi_transfer(data);
}

void good_display_base::reset() {
  digital_write(settings::m_pin_config.epd_rst_pin, LOW);                //module reset
  delay_ms(10);
  digital_write(settings::m_pin_config.epd_rst_pin, HIGH);
  delay_ms(10);
}
