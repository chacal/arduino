#include "button_handler.h"
#include "custom_board.h"
#include <app_button.h>
#include <nrf_log.h>
#include <app_timer.h>
#include "internal_command.h"
#include <app_scheduler.h>
#include "display_list.h"
#include "marinedisplay.pb.h"

#define DEBOUNCE           APP_TIMER_TICKS(50, 0)
#define TICKS_IN_SEC       APP_TIMER_TICKS(1000, 0)
#define MENU_ROW_HEIGHT    16
#define FONT_SIZE          10
#define CMD_NAME_X_COORD   20
#define INDICATOR_X_COORD   5

typedef struct {
  uint32_t       delay;
  internal_cmd_t cmd;
  char           cmd_name[15];
} button_cmd_t;


static button_cmd_t m_button_commands[] = {
    {APP_TIMER_TICKS(3000, 0), DISCOVERABLE,   "Discoverable"},
    {APP_TIMER_TICKS(6000, 0), SETTINGS_RESET, "Factory Reset"},
    {APP_TIMER_TICKS(0, 0),    NOOP,           "Cancel"}
};
static uint8_t      m_cmd_count         = sizeof(m_button_commands) / sizeof(m_button_commands[0]);
static uint32_t     m_btn_press_time;
APP_TIMER_DEF(m_second_timer);


static uint8_t get_currently_selected_command(uint32_t *secs_till_next) {
  uint32_t duration = 0;
  uint8_t  idx      = 0;

  app_timer_cnt_diff_compute(app_timer_cnt_get(), m_btn_press_time, &duration);

  for(; idx < m_cmd_count; ++idx) {
    if(duration < m_button_commands[idx].delay) {
      uint32_t ticks_till_next = m_button_commands[idx].delay - duration;
      *secs_till_next = (ticks_till_next + TICKS_IN_SEC - 1) / TICKS_IN_SEC;  // Round up
      return idx;
    }
  }

  *secs_till_next = 0;
  return (uint8_t) (m_cmd_count - 1);  // Special case, last item is selected if time for all others have already run out
}


static void render_menu(void *p_context) {
  uint32_t secs_till_next;
  uint8_t  current_cmd = get_currently_selected_command(&secs_till_next);

  display_list_clear();

  // Render command names
  for(uint8_t i = 0; i < m_cmd_count; ++i) {
    DisplayCommand cmd = {
        .which_command = DisplayCommand_string_tag,
        .command.string = {i, CMD_NAME_X_COORD, (uint8_t) ((i + 1) * MENU_ROW_HEIGHT), FONT_SIZE}
    };
    sprintf(cmd.command.string.str, "%s", m_button_commands[i].cmd_name);
    display_list_add(i, &cmd);
  }

  // Render indicator for currently selected command
  DisplayCommand indicator = {
      .which_command = DisplayCommand_string_tag,
      .command.string = {m_cmd_count, INDICATOR_X_COORD, (uint8_t) ((current_cmd + 1) * MENU_ROW_HEIGHT), FONT_SIZE}
  };
  if(current_cmd == m_cmd_count - 1) {    // Last command is selected -> show static indicator
    strcpy(indicator.command.string.str, ">");
  } else {
    sprintf(indicator.command.string.str, "%d", (int) secs_till_next);
  }
  display_list_add(m_cmd_count, &indicator);

  SCHED_INT_CMD(RENDER);
}


static void start_selecting_command() {
  m_btn_press_time = app_timer_cnt_get();
  app_timer_start(m_second_timer, TICKS_IN_SEC, NULL);
  SCHED_INT_CMD(DISPLAY_ON);
  render_menu(NULL);
}

static void execute_selected_command() {
  display_list_clear();
  SCHED_INT_CMD(RENDER);

  uint32_t secs_till_next;
  uint8_t  cmd_count = get_currently_selected_command(&secs_till_next);
  SCHED_INT_CMD(m_button_commands[cmd_count].cmd);

  app_timer_stop(m_second_timer);
}

static void button_handler(uint8_t pin_no, uint8_t button_action) {
  if(button_action == APP_BUTTON_PUSH) {
    start_selecting_command();
  } else {
    execute_selected_command();
  }
}

void btn_handler_init() {
  static app_button_cfg_t cfg = {BUTTON_1, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, button_handler};
  APP_ERROR_CHECK(app_button_init(&cfg, 1, DEBOUNCE));
  APP_ERROR_CHECK(app_button_enable());
  app_timer_create(&m_second_timer, APP_TIMER_MODE_REPEATED, render_menu);
}