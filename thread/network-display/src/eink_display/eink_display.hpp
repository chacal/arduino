#pragma once

#include <memory>
#include <display.hpp>
#include <eink_display/good_display/good_display_base.hpp>
#include <eink_display/good_display/good_display_1in54.hpp>

class eink_display : public display {
public:
  eink_display();

  void on() override;

  void render() override;

  void clear() override;

private:
  good_display_base          epd      = good_display_1in54{};
  std::unique_ptr<uint8_t[]> u8g2_buf = std::make_unique<uint8_t[]>(epd.width * epd.height / 8);
  std::unique_ptr<uint8_t[]> epd_buf  = std::make_unique<uint8_t[]>(epd.width * epd.height / 8);
};
