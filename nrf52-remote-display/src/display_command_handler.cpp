#include "display_command_handler.hpp"

void display_command_handler::operator()(const commands::str_cmd &s) {
  NRF_LOG_DEBUG("STR: %s", s.str.c_str())
  dl.add(s);
}

void display_command_handler::operator()(const commands::line_cmd &l) {
  NRF_LOG_DEBUG("LINE: (%d,%d) -> (%d,%d)", l.start.x, l.start.y, l.end.x, l.end.y)
  dl.add(l);
}

void display_command_handler::operator()(const commands::clear_cmd &c) {
  NRF_LOG_DEBUG("CLEAR")
  dl.clear();
}
