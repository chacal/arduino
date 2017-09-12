#include "button_handler.h"
#include "custom_board.h"
#include <app_button.h>
#include <nrf_log.h>
#include <app_timer.h>
#include "internal_command.h"


#define DEBOUNCE           APP_TIMER_TICKS(50, 0)


typedef struct {
  uint32_t delay;
  internal_cmd_t cmd;
  char cmd_name[15];
} button_cmd_t;

static uint32_t m_btn_press_time;
APP_TIMER_DEF(m_second_timer);


static button_cmd_t m_button_commands[] = {
    {APP_TIMER_TICKS(3000, 0), DISPLAY_ON,  "Display On"},
    {APP_TIMER_TICKS(6000, 0), DISPLAY_OFF, "Display Off"},
    {APP_TIMER_TICKS(9000, 0), RENDER,      "Render"}
};


static uint8_t get_currently_selected_commands(button_cmd_t *current_cmd, button_cmd_t *next_cmd, uint32_t *ticks_till_next) {
  uint32_t duration = 0;
  uint8_t cmd_count = sizeof(m_button_commands)/sizeof(m_button_commands[0]);
  uint8_t ret = 0;

  app_timer_cnt_diff_compute(app_timer_cnt_get(), m_btn_press_time, &duration);

  for(int i = 0; i < cmd_count; ++i) {
    if(duration < m_button_commands[i].delay) {
      memcpy(current_cmd, &m_button_commands[i], sizeof(m_button_commands[i]));
      *ticks_till_next = m_button_commands[i].delay - duration;
      ret++;
      if(i < cmd_count - 1) {
        memcpy(next_cmd, &m_button_commands[i+1], sizeof(m_button_commands[i+1]));
        ret++;
      }
      break;
    }
  }
  return ret;
}


static void show_current_and_next_commands(void *p_context) {
  button_cmd_t current_cmd;
  button_cmd_t next_cmd;
  uint32_t     ticks_till_next;

  uint8_t cmd_count = get_currently_selected_commands(&current_cmd, &next_cmd, &ticks_till_next);
  if(cmd_count == 2) {
    NRF_LOG_INFO("Current: %s     Next in %d: %s\n", (uint32_t)current_cmd.cmd_name, ticks_till_next, (uint32_t)next_cmd.cmd_name);
  } else if(cmd_count == 1) {
    NRF_LOG_INFO("Current: %s     Next in %d: Cancel\n", (uint32_t)current_cmd.cmd_name, ticks_till_next);
  } else {
    NRF_LOG_INFO("Current: Cancel\n");
  }
}


static void start_selecting_command() {
  m_btn_press_time = app_timer_cnt_get();
  app_timer_start(m_second_timer, APP_TIMER_TICKS(1000, 0), NULL);
  show_current_and_next_commands(NULL);
}

static void execute_selected_command() {
  button_cmd_t current_cmd;
  button_cmd_t next_cmd;
  uint32_t     ticks_till_next;

  uint8_t cmd_count = get_currently_selected_commands(&current_cmd, &next_cmd, &ticks_till_next);
  if(cmd_count > 0) {
    NRF_LOG_INFO("Executing: %s\n", (uint32_t)current_cmd.cmd_name);
  }

  uint32_t ret = app_timer_stop(m_second_timer);
  NRF_LOG_INFO("Return: %d\n", ret);
}

static void button_handler(uint8_t pin_no, uint8_t button_action) {
  if(button_action == APP_BUTTON_PUSH) {
    start_selecting_command();
  } else {
    execute_selected_command();
  }
}

void btn_handler_init() {
  static app_button_cfg_t cfg = { BUTTON_1, APP_BUTTON_ACTIVE_LOW, NRF_GPIO_PIN_PULLUP, button_handler };
  APP_ERROR_CHECK(app_button_init(&cfg, 1, DEBOUNCE));
  APP_ERROR_CHECK(app_button_enable());
  app_timer_create(&m_second_timer, APP_TIMER_MODE_REPEATED, show_current_and_next_commands);
}