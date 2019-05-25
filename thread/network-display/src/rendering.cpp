#include <nrf_log.h>
#include "rendering.hpp"

namespace rendering {
  void display_list::add(const display_list_command &new_cmd) {
    std::visit([this](const auto &cmd) {
      if(cmd.index < DISPLAY_LIST_LENGTH) {
        commands.at(cmd.index) = cmd;
        dirty = true;
      } else {
        NRF_LOG_WARNING("Display command index out of bounds! index: %d, max allowed: %d", cmd.index, commands.size() - 1)
      }
    }, new_cmd);
  }

  void display_list::clear() {
    for(auto &i : commands) {
      i = std::monostate{};
    }
    dirty = true;
  }

  void display_list::render(display &d) {
    if(! dirty) {
      return;
    }

    d.clear();

    for(auto &cmd : commands) {
      if(auto str = std::get_if<commands::str_cmd>(&cmd)) {
        d.draw_str(str->start, str->size, str->str);
      } else if(auto line = std::get_if<commands::line_cmd>(&cmd)) {
        d.draw_line(line->start, line->end);
      }
    }

    d.render();
    dirty = false;
  }
}