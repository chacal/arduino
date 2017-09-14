#pragma once

typedef enum {
  DISPLAY_ON,
  DISPLAY_OFF,
  RENDER,
  DISCOVERABLE,
  FACTORY_RESET,
  NOOP
} internal_cmd_t;


#define SCHED_INT_CMD(COMMAND)                                                                                       \
  {                                                                                                                  \
    internal_cmd_t cmd = COMMAND;                                                                                    \
    APP_ERROR_CHECK(app_sched_event_put(&cmd, sizeof(internal_cmd_t), (app_sched_event_handler_t)on_internal_cmd));  \
  }


void on_internal_cmd(internal_cmd_t const *p_data, uint16_t length);