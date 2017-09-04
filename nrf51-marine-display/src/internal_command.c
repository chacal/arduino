#include <nrf_log.h>
#include "internal_command.h"
#include "display.h"


void on_internal_cmd(internal_cmd_t const *p_cmd, uint16_t length) {
  internal_cmd_t cmd = *p_cmd;

  switch (cmd) {
    case DISPLAY_ON:
      display_on();
      break;
    case DISPLAY_OFF:
      display_off();
      break;
    default:
      NRF_LOG_WARNING("Got unknown internal command! %d", cmd);
  }
}
