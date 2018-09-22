#pragma once

#include "util.hpp"
#include "display.hpp"

namespace rendering {
  void render_discoverable_state(const display &, const util::seconds &time_left);
}