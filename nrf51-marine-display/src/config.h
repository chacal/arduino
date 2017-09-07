#pragma once

#define PB_CMD_MAX_SIZE            35  // Enough to transfer 20 character string
#define PB_CMD_SEQ_MAX_LENGTH       5
#define MAX_BLE_MESSAGE_SIZE        (PB_CMD_MAX_SIZE * PB_CMD_SEQ_MAX_LENGTH)

#define APP_SCHEDULER_QUEUE_SIZE    (PB_CMD_SEQ_MAX_LENGTH + 1)
#define APP_SCHEDULER_EVENT_SIZE    sizeof(DisplayCommand)

#define DISPLAY_LIST_LENGTH        10

#define GATT_ATTR_TABLE_SIZE      420  // Minimum needed - found by trial & error