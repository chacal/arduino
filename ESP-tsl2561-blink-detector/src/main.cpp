#include <Arduino.h>

#include "wifi.hpp"
#include "tsl2561.hpp"
#include "pulse_detector.hpp"

/*
 * Wiring TSL2561 to Wemos D1 Mini:
 * VCC  ->  3V3
 * GND  ->  G
 * SCL  ->  D1
 * SDA  ->  D2
 * INT  ->  D5
 */

void blinkLed() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(2);
  digitalWrite(LED_BUILTIN, HIGH);
}

void on_pulse_detected() {
  static uint32_t m_pulse_counter = 0;
  sendPacket(++m_pulse_counter);
  blinkLed();
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting ESP blink detector");
  connectWifi();
  tsl2561_init();
  pulse_detector_init(on_pulse_detected);
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
}
