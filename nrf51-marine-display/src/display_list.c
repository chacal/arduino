#include <nrf_log.h>
#include "marinedisplay.pb.h"
#include "config.h"
#include "display_list.h"
#include "display.h"

static DisplayCommand m_display_list[DISPLAY_LIST_LENGTH];
static bool           m_display_list_set[DISPLAY_LIST_LENGTH];
static bool           m_dirty;


static void add_to_list(uint8_t idx, DisplayCommand *p_cmd) {
  if(idx < DISPLAY_LIST_LENGTH) {
    memcpy(&m_display_list[idx], p_cmd, sizeof(DisplayCommand));
    m_display_list_set[idx] = true;
    m_dirty = true;
  } else {
    NRF_LOG_ERROR("Display list index out of bounds! Got idx: %d List length: %d\n", idx, DISPLAY_LIST_LENGTH);
  }
}

void display_list_add(uint8_t idx, DisplayCommand *p_cmd) {
  DisplayCommand cmd = *p_cmd;
  if(cmd.which_command == DisplayCommand_string_tag || cmd.which_command == DisplayCommand_line_tag) {
    add_to_list(idx, &cmd);
  } else {
    NRF_LOG_WARNING("Unknown command for display list: %d\n", cmd.which_command);
  }
}

void display_list_clear() {
  memset(m_display_list_set, false, DISPLAY_LIST_LENGTH);
  m_dirty = true;
}

static void render_cmd(DisplayCommand *cmd) {
  switch (cmd->which_command) {
    case DisplayCommand_string_tag:
      display_draw_str(cmd->command.string.x, cmd->command.string.y, cmd->command.string.font_size, cmd->command.string.str);
      break;
    case DisplayCommand_line_tag:
      display_draw_line(cmd->command.line.start_x, cmd->command.line.start_y, cmd->command.line.end_x, cmd->command.line.end_y);
      break;
    default:
      break;
  }
}

void display_list_render() {
  if(!m_dirty) {
    return;
  }

  display_clear();
  for(int i = 0; i < DISPLAY_LIST_LENGTH; ++i) {
    if(m_display_list_set[i]) {
      render_cmd(&m_display_list[i]);
    }
  }
  display_render();
  m_dirty = false;
}