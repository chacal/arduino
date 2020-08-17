#include <Ticker.h>

#include "wifi.hpp"

Ticker blinker;

void connectWiFi(WiFiManager &wifiManager, const String &hostname) {
  Serial.println("Connecting to WiFi..");
  WiFi.hostname(hostname);

  wifiManager.setAPCallback([](WiFiManager *wifiManager) {
    blinker.attach_ms(500, []() { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); });
  });

  wifiManager.setSaveConfigCallback([]() {
    blinker.detach();
    digitalWrite(LED_BUILTIN, HIGH);
  });

  wifiManager.setConfigPortalTimeout(180);

  if (!wifiManager.autoConnect("fan-control-setup")) {
    Serial.println("Timeout while trying to establish WiFi connection. Rebooting..");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  Serial.println(String("Connected to WiFi: ") + WiFi.SSID());
  Serial.println(String("IP address: ") + WiFi.localIP().toString());
}
