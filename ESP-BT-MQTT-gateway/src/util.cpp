#include <Arduino.h>
#include <StreamPrint.h>
#include <WiFiManager.h>

#include "config.hpp"


int readline(int readch, char *buffer, int buf_len) {
  static int pos = 0;
  int        rpos;

  if (readch > 0) {
    switch (readch) {
      case '\r': // Ignore CR new-lines
        break;
      case '\n': // Return on new-lines
        rpos = pos;
        pos  = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < buf_len - 1) {
          buffer[pos++] = readch;
          buffer[pos]   = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}


void resetConfigAndReboot(WiFiManager &wifiManager) {
  Serial << "Resetting configuration." << endl;
  removeSavedConfig();
  wifiManager.resetSettings();
  delay(1000);
  Serial << "Rebooting.." << endl;
  delay(2000);
  ESP.restart();
}

void blinkLed(uint8_t times) {
  pinMode(LED_BUILTIN, OUTPUT);

  for (uint8_t i = 0; i < times; ++i) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(2);
    digitalWrite(LED_BUILTIN, HIGH);
    if (times > 1) {
      delay(100);
    }
  }
}

void resetAndRebootIfPinLow(uint8_t pin, WiFiManager &wifiManager) {
  pinMode(pin, INPUT_PULLUP);
  if (digitalRead(pin) == LOW) {
    blinkLed(20);
    resetConfigAndReboot(wifiManager);
  }
}
