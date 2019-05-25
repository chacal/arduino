#pragma once

#include <memory>
#include <display.hpp>
#include <eink_display/epd_2in9/epd_2in9.hpp>

class eink_display : public display {
public:
  eink_display();

  void on() override;

  void render() override;

  void clear() override;

private:
  epd_2in9                   epd;
  std::unique_ptr<uint8_t[]> u8g2_buf = std::make_unique<uint8_t[]>(epd.width * epd.height / 8);
  std::unique_ptr<uint8_t[]> epd_buf  = std::make_unique<uint8_t[]>(epd.width * epd.height / 8);
};
