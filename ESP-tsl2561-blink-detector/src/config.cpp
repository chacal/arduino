#include "config.hpp"
#include <FS.h>

#define DST_HOST               "rapuserver.chacal.fi"
#define PULSE_REPORTING_PORT   5555
#define SAMPLES_REPORTING_PORT 5556
#define REPORT_SAMPLES         false
#define MAX_PULSE_LENGTH_MS    40
#define PULSE_START_COEF       1.8f
#define PULSE_END_COEF         1.3f

#define CONFIG_FILE_NAME       "config.json"

Config config = {
    DST_HOST,
    PULSE_REPORTING_PORT,
    SAMPLES_REPORTING_PORT,
    REPORT_SAMPLES,
    MAX_PULSE_LENGTH_MS,
    PULSE_START_COEF,
    PULSE_END_COEF
};

void load_config_from_file() {
  SPIFFS.begin();
  File                    config_file = SPIFFS.open(CONFIG_FILE_NAME, "r");
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc, config_file);
  if (error) {
    Serial.println("Failed to read config file, using default configuration");
    print_config();
    save_config_to_file();
  } else {
    update_config_from_json(doc.as<JsonVariant>());
  }

  config_file.close();
}

void save_config_to_file() {
  SPIFFS.begin();
  auto file = SPIFFS.open(CONFIG_FILE_NAME, "w");

  if (!file) {
    Serial.println("Failed to create file!");
    return;
  }

  serializeJson(get_config_as_json(), file);
  file.close();
}

void update_config_from_json(const JsonVariant &doc) {
  if (doc.containsKey("dstHost") && doc["dstHost"].is<char *>()) {
    config.dst_host = doc["dstHost"].as<String>();
  }

  config.pulse_reporting_port   = doc["pulseReportingPort"] | config.pulse_reporting_port;
  config.samples_reporting_port = doc["samplesReportingPort"] | config.samples_reporting_port;
  config.report_samples         = doc["reportSamples"] | config.report_samples;
  config.max_pulse_length       = doc["maxPulseLength"] | config.max_pulse_length;
  config.pulse_start_coef       = doc["pulseStartCoef"] | config.pulse_start_coef;
  config.pulse_end_coef         = doc["pulseEndCoef"] | config.pulse_end_coef;

  print_config();
}

DynamicJsonDocument get_config_as_json() {
  DynamicJsonDocument doc(512);
  doc["dstHost"]              = config.dst_host;
  doc["pulseReportingPort"]   = config.pulse_reporting_port;
  doc["samplesReportingPort"] = config.samples_reporting_port;
  doc["reportSamples"]        = config.report_samples;
  doc["maxPulseLength"]       = config.max_pulse_length;
  doc["pulseStartCoef"]       = config.pulse_start_coef;
  doc["pulseEndCoef"]         = config.pulse_end_coef;
  return doc;
}

void print_config() {
  Serial.println("Using config:\n-------------");
  Serial.printf("Destination: \t\t%s\n", config.dst_host.c_str());
  Serial.printf("Pulse reporting port: \t%u\n", config.pulse_reporting_port);
  Serial.printf("Samples reporting port: %u\n", config.samples_reporting_port);
  Serial.printf("Report samples: \t%s\n", config.report_samples ? "true" : "false");
  Serial.printf("Max pulse length: \t%u\n", config.max_pulse_length);
  Serial.printf("Pulse start coef: \t%f\n", config.pulse_start_coef);
  Serial.printf("Pulse end coef: \t%f\n\n", config.pulse_end_coef);
}
