#include <nrf_log.h>
#include <peer_manager.h>
#include "internal_command.h"
#include "display.h"
#include "display_list.h"
#include "power_manager.h"
#include "ble_support.h"


void on_internal_cmd(internal_cmd_t const *p_cmd, uint16_t length) {
  internal_cmd_t cmd = *p_cmd;

  switch (cmd) {
    case DISPLAY_ON:
      display_on();
      break;
    case DISPLAY_OFF:
      display_off();
      break;
    case RENDER:
      display_list_render();
      break;
    case POWER_OFF:
      display_off();
      ble_support_disconnect(power_manager_shutdown);
      break;
    case DISCOVERABLE:
      break;
    case FACTORY_RESET:
      NRF_LOG_INFO("Factory reset\n")
      APP_ERROR_CHECK(pm_peers_delete());
      ble_support_disconnect(NULL);
      break;
    case NOOP:
      break;
    default:
      NRF_LOG_WARNING("Got unknown internal command! %d", cmd);
  }
}
