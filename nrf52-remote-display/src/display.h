#pragma once

#include <stdint.h>

void display_init();
void display_on();
void display_off();
void display_draw_str(uint8_t x, uint8_t y, uint8_t font_size, char *str);
void display_draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y);
void display_render();
void display_clear();
uint8_t display_centered_x(const char *str, uint8_t font_size);
