#include <nrf_log.h>
#include "rendering.hpp"

namespace rendering {
  void render_discoverable_state(const display &disp, const util::seconds &time_left) {
    NRF_LOG_INFO("Discoverable %us", time_left.get())
  }
}