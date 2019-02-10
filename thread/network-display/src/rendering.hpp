#pragma once

#include <variant>
#include <vector>
#include "util.hpp"
#include "display.hpp"
#include "cmd_protocol.hpp"

#define DISPLAY_LIST_LENGTH      20

namespace rendering {
  using display_list_command = std::variant<commands::str_cmd, commands::line_cmd>;

  class display_list {
  public:
    void add(const display_list_command &);

    void clear();

    void render(display &);

    const uint8_t max_length = DISPLAY_LIST_LENGTH;

  private:
    // std::monostate represents an empty place in the display list
    using display_list_command_internal = std::variant<std::monostate, commands::str_cmd, commands::line_cmd>;

    bool                                       dirty = false;
    std::vector<display_list_command_internal> commands{DISPLAY_LIST_LENGTH};
  };
}