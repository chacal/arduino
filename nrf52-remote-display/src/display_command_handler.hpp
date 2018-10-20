#pragma once

#include "cmd_protocol.hpp"

class display_command_handler {
public:
  void operator()(const commands::str_cmd &);

  void operator()(const commands::line_cmd &);

  void operator()(const commands::clear_cmd &);
};