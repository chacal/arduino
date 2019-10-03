#pragma once

#include <Arduino.h>

#define DST_PORT           5555
#define UDP_SERVER_PORT    5555

struct Config {
  String   dst_host;
  uint32_t max_pulse_length;
  float_t  pulse_start_coef;
  float_t  pulse_end_coef;
};

extern Config config;

void update_config_from_json(const String &);

void print_config();
