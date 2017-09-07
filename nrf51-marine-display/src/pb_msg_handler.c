#include "pb_msg_handler.h"
#include <pb_decode.h>
#include <nrf_log.h>
#include <app_scheduler.h>
#include <softdevice/s130/headers/nrf_soc.h>
#include "marinedisplay.pb.h"
#include "display_list.h"
#include "power_manager.h"
#include "internal_command.h"

static bool decode_single_command(pb_istream_t *stream, const pb_field_t *field, void **arg) {
  DisplayCommand cmd = DisplayCommand_init_zero;
  bool status = pb_decode(stream, DisplayCommand_fields, &cmd);

  if (!status) {
    const char *err_msg = PB_GET_ERROR(stream);
    NRF_LOG_WARNING("Decoding DisplayCommand failed: %s\n", (uint32_t)err_msg);
  } else {
    APP_ERROR_CHECK(app_sched_event_put(&cmd, sizeof(cmd), pb_msg_handle));
  }

  return status;
}

void pb_msg_decode(void *p_data, uint16_t length) {
  CommandSeq cmd_seq = CommandSeq_init_zero;
  cmd_seq.commands.funcs.decode = decode_single_command;

  pb_istream_t stream = pb_istream_from_buffer(p_data, length);
  bool status = pb_decode(&stream, CommandSeq_fields, &cmd_seq);

  if (!status) {
    const char *err_msg = PB_GET_ERROR(&stream);
    NRF_LOG_WARNING("Decoding CommandSeq failed: %s\n", (uint32_t) err_msg);
  }

  SCHED_INT_CMD(RENDER);
}

static void enter_bootloader() {
  NRF_LOG_INFO("Entering bootloader..\n");
  sd_power_gpregret_clr(0xFFFFFFFF);
  sd_power_gpregret_set(1);
  NVIC_SystemReset();
}

void pb_msg_handle(void *p_data, uint16_t length) {
  DisplayCommand *cmd = p_data;

  switch(cmd->which_command) {
    case DisplayCommand_string_tag:
      NRF_LOG_INFO("String cmd: %s\n", (uint32_t) cmd->command.string.str);
      display_list_add(cmd->command.string.index, cmd);
      break;
    case DisplayCommand_line_tag:
      NRF_LOG_INFO("Line cmd: (%d,%d) -> (%d,%d)\n", cmd->command.line.start_x, cmd->command.line.start_y, cmd->command.line.end_x, cmd->command.line.end_y);
      display_list_add(cmd->command.line.index, cmd);
      break;
    case DisplayCommand_clear_tag:
      NRF_LOG_INFO("Clear cmd\n");
      display_list_clear();
      break;
    case DisplayCommand_shutdown_tag:
      NRF_LOG_INFO("Shutdown cmd\n");
      power_manager_shutdown();
      break;
    case DisplayCommand_bootloader_tag:
      enter_bootloader();
      break;
    default:
      NRF_LOG_WARNING("Unknown command tag: %d\n", cmd->which_command);
      return;
  }
}