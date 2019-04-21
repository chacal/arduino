#pragma once

#include <memory>
#include <vector>
#include <variant>
#include <nrf_log.h>
#include "display.hpp"
#include "coap_service.hpp"

namespace commands {

  struct str_cmd {
    uint8_t     index;
    point       start;
    font_size   size;
    std::string str;
  };

  struct line_cmd {
    uint8_t index;
    point   start;
    point   end;
  };

  struct clear_cmd {};

  using display_command = std::variant<str_cmd, line_cmd, clear_cmd>;
  using display_command_seq = std::vector<display_command>;

  const std::unique_ptr<display_command_seq> parse(const coap_service::post_data &);

}