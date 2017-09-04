#pragma once

#include <stdint.h>

void display_init();
void display_on();
void display_off();
void display_draw_str(uint32_t x, uint32_t y, uint32_t font_size, char *str);
void display_draw_line(uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y, uint32_t width);
void display_render();
void display_clear();
