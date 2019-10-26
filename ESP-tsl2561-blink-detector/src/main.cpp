#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>

#include "config.hpp"
#include "wifi.hpp"
#include "tsl2561.hpp"
#include "pulse_detector.hpp"
#include "web_server.hpp"

/*
 * Wiring TSL2561 to Wemos D1 Mini:
 * VCC  ->  3V3
 * GND  ->  G
 * SCL  ->  D1
 * SDA  ->  D2
 * INT  ->  D5
 */

#define INSTANCE   "I100"

void blinkLed() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(2);
  digitalWrite(LED_BUILTIN, HIGH);
}

String impulse_json_for(uint32_t counter) {
  StaticJsonDocument<255> doc;
  doc["instance"] = INSTANCE;
  doc["counter"]  = counter;
  return doc.as<String>();
}

void on_pulse_detected() {
  static uint32_t m_pulse_counter = 0;
  blinkLed();
  sendPacket(impulse_json_for(++m_pulse_counter), config.dst_host, config.pulse_reporting_port);
}

void on_full_adc_buffer() {
  if (config.report_samples) {
    sendPacket([](UDP &udp) {
      pulse_detector_write_samples(udp);
    }, config.dst_host, config.samples_reporting_port);
  }
}

void on_tick() {
  web_server_broadcast_ws(String(pulse_detector_current_avg()));
}

Ticker t;

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting ESP blink detector");
  load_config_from_file();

  connectWifi();

  auto hostname = String(INSTANCE);
  hostname.toLowerCase();
  MDNS.begin(hostname);
  ArduinoOTA.begin();

  web_server_init();

  tsl2561_init();
  pulse_detector_init(on_pulse_detected, on_full_adc_buffer);

  t.attach_scheduled(1, on_tick);

  pinMode(LED_BUILTIN, OUTPUT);
  // Indicate that Wifi connection is established
  for (int i = 0; i < 8; ++i) {
    blinkLed();
    delay(100);
  }
}

void loop() {
  if (tsl2561_interrupt_pending()) {
    pulse_detector_process(tsl2561_get_adc0_value());
    tsl2561_clear_interrupt();
  }
  ArduinoOTA.handle();
}
