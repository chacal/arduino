#pragma once

#include <chrono>
#include <memory>
#include <display.hpp>
#include "coap_service.hpp"

#define MGMT_SERVER_PORT      5683

using namespace std::chrono;

namespace settings {
  enum DisplayType {
    GOOD_DISPLAY_1_54IN,
    GOOD_DISPLAY_2_13IN,
    GOOD_DISPLAY_2_9IN,
    GOOD_DISPLAY_2_9IN_4GRAY
  };

  enum HardwareVersion {
    E73,
    MS88SF2_V1_0,
  };

  struct pin_config {
    uint8_t epd_rst_pin;
    uint8_t epd_dc_pin;
    uint8_t epd_busy_pin;
    uint8_t epd_cs_pin;
    uint8_t epd_reset_ctl1_pin;
    uint8_t epd_reset_ctl2_pin;
    uint8_t spi_clk_pin;
    uint8_t spi_mosi_pin;
    uint8_t spi_miso_pin;
    uint8_t lis3dh_cs_pin;
    uint8_t lis3dh_int1_pin;
    uint8_t lis3dh_int2_pin;
  };

  const pin_config E73_BOARD_PINOUT = {
    .epd_rst_pin = 31,
    .epd_dc_pin = 29,
    .epd_busy_pin = 30,
    .epd_cs_pin = 2,
    .epd_reset_ctl1_pin = 0,
    .epd_reset_ctl2_pin = 0,
    .spi_clk_pin = 28,
    .spi_mosi_pin = 3,
    .spi_miso_pin = 4,
    .lis3dh_cs_pin = 0,
    .lis3dh_int1_pin = 0,
    .lis3dh_int2_pin = 0
  };

  const pin_config MS88SF2_BOARD_PINOUT = {
    .epd_rst_pin = 2,
    .epd_dc_pin = 8,
    .epd_busy_pin = 29,
    .epd_cs_pin = 12,
    .epd_reset_ctl1_pin = 41, // P1.09
    .epd_reset_ctl2_pin = 13,
    .spi_clk_pin = 4,
    .spi_mosi_pin = 26,
    .spi_miso_pin = 6,
    .lis3dh_cs_pin = 15,
    .lis3dh_int1_pin = 22,
    .lis3dh_int2_pin = 20
  };

  extern std::string                    m_instance;
  extern std::string                    m_mgmt_server;
  extern std::optional<DisplayType>     m_display_type;
  extern std::optional<HardwareVersion> m_hw_version;
  extern pin_config                     m_pin_config;

  struct settings {
    std::optional<std::string>     instance;
    std::optional<int8_t>          txPower;
    std::optional<milliseconds>    pollPeriod;
    std::optional<milliseconds>    increasedPollPeriod;
    std::optional<milliseconds>    increasedPollDuration;
    std::optional<DisplayType>     displayType;
    std::optional<HardwareVersion> hwVersion;
  };

  void updateFromCoapData(const coap_service::post_data &coap_data);

  void update(const uint8_t *data, uint32_t len);

  std::string get_as_json();

  std::optional<settings> fromJson(const uint8_t *jsonData, uint32_t len);

  std::unique_ptr<display> createDisplay(DisplayType displayType);

  std::string toString(std::optional<DisplayType> displayType);

  std::optional<DisplayType> displayTypeFromString(const std::string &str);

  std::string toString(std::optional<HardwareVersion> hwVersion);

  std::optional<HardwareVersion> hwVersionFromString(const std::string &str);
}