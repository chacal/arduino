#pragma once

#include "util.hpp"
#include "display.hpp"

namespace rendering {
  void render_discoverable_state(const display &, const std::chrono::seconds &time_left);
}