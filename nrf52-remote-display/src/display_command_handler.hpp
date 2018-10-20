#pragma once

#include "cmd_protocol.hpp"
#include "rendering.hpp"

class display_command_handler {
public:
  display_command_handler(rendering::display_list &dl) : dl(dl) {}

  void operator()(const commands::str_cmd &);

  void operator()(const commands::line_cmd &);

  void operator()(const commands::clear_cmd &);

private:
  rendering::display_list &dl;
};