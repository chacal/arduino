#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

struct Config {
  String   dst_host;
  uint16_t pulse_reporting_port;
  uint16_t samples_reporting_port;
  boolean  report_samples;
  uint32_t max_pulse_length;
  float_t  pulse_start_coef;
  float_t  pulse_end_coef;
};

extern Config config;

void update_config_from_json(const JsonVariant &json);

DynamicJsonDocument get_config_as_json();

void print_config();
