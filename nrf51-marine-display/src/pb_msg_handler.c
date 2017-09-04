#include "pb_msg_handler.h"
#include <pb_decode.h>
#include <nrf_log.h>
#include "marinedisplay.pb.h"
#include "display.h"

void pb_msg_handle(void *p_data, uint16_t length) {
  DisplayCommand cmd = DisplayCommand_init_zero;
  pb_istream_t stream = pb_istream_from_buffer(p_data, length);

  bool status = pb_decode(&stream, DisplayCommand_fields, &cmd);

  if (!status) {
    const char *err_msg = PB_GET_ERROR(&stream);
    NRF_LOG_WARNING("PB decoding failed: %s\n", (uint32_t)err_msg);
  } else {
    switch (cmd.which_command) {
      case DisplayCommand_string_tag:
        NRF_LOG_INFO("String cmd: %s\n", (uint32_t)cmd.command.string.str);
        display_render_str(1,
                           cmd.command.string.x,
                           cmd.command.string.y,
                           cmd.command.string.font_size,
                           cmd.command.string.str);
        break;
      case DisplayCommand_line_tag:
        NRF_LOG_INFO("Line cmd: (%d,%d) -> (%d,%d)\n", cmd.command.line.start_x, cmd.command.line.start_y, cmd.command.line.end_x, cmd.command.line.end_y);
        display_render_line(1,
                            cmd.command.line.start_x,
                            cmd.command.line.start_y,
                            cmd.command.line.end_x,
                            cmd.command.line.end_y,
                            cmd.command.line.width);
        break;
      case DisplayCommand_clear_tag:
        NRF_LOG_INFO("Clear cmd\n");
        display_clear();
        break;
      default:
        NRF_LOG_WARNING("Unknown command tag: %d\n", cmd.which_command);
        break;
    }
  }
}