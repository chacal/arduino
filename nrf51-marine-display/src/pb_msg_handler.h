#pragma once

#include <stdint.h>

void pb_msg_decode(void *p_data, uint16_t length);
void pb_msg_handle(void *p_data, uint16_t length);