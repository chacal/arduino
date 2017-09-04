#pragma once

#include "marinedisplay.pb.h"

void display_list_add(uint8_t idx, DisplayCommand *cmd);
void display_list_clear();
void display_list_render();