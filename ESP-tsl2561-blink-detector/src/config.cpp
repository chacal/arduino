#include <ArduinoJson.h>

#include "config.hpp"

#define DST_HOST               "rapuserver.chacal.fi"
#define MAX_PULSE_LENGTH_MS    200
#define PULSE_START_COEF       1.8f
#define PULSE_END_COEF         1.3f

Config config = {
    DST_HOST,
    MAX_PULSE_LENGTH_MS,
    PULSE_START_COEF,
    PULSE_END_COEF
};

void update_config_from_json(const String &config_json) {
  StaticJsonDocument<512> doc;
  DeserializationError    error = deserializeJson(doc, config_json);

  if (error) {
    Serial.println("Failed to parse configuration JSON!");
    Serial.println("Received:");
    Serial.println(config_json);
    return;
  }

  if (doc.containsKey("dstHost") && doc["dstHost"].is<char *>()) {
    config.dst_host = doc["dstHost"].as<String>();
  }

  config.max_pulse_length = doc["maxPulseLength"] | config.max_pulse_length;
  config.pulse_start_coef = doc["pulseStartCoef"] | config.pulse_start_coef;
  config.pulse_end_coef   = doc["pulseEndCoef"] | config.pulse_end_coef;

  print_config();
}

void print_config() {
  Serial.println("Using config:\n-------------");
  Serial.printf("Destination: \t\t%s\n", config.dst_host.c_str());
  Serial.printf("Max pulse length: \t%u\n", config.max_pulse_length);
  Serial.printf("Pulse start coef: \t%f\n", config.pulse_start_coef);
  Serial.printf("Pulse end coef: \t%f\n\n", config.pulse_end_coef);
}
