#include <Ticker.h>
#include <StreamPrint.h>

#include "wifi.hpp"
#include "config.hpp"

bool   configurationSaved = false;
Ticker blinker;

void connectWiFi(WiFiManager &wifiManager) {
  Serial << "Connecting to WiFi.." << endl;
  configurationSaved = false;
  WiFi.hostname(config.hostname);

  WiFiManagerParameter mqttServerParam("mqtt_server", "MQTT server", config.mqttServer.c_str(), 100);
  WiFiManagerParameter mqttPortParam("mqtt_port", "MQTT port", String(config.mqttPort).c_str(), 6);
  WiFiManagerParameter mqttUsernameParam("mqtt_username", "MQTT username", config.mqttUsername.c_str(), 100);
  WiFiManagerParameter mqttPasswordParam("mqtt_password", "MQTT password", config.mqttPassword.c_str(), 100);
  WiFiManagerParameter mqttTopicParam("mqtt_topic", "MQTT topic", config.mqttTopic.c_str(), 100);
  WiFiManagerParameter hostnameParam("hostname", "MDNS hostname", config.hostname.c_str(), 100);

  wifiManager.addParameter(&mqttServerParam);
  wifiManager.addParameter(&mqttPortParam);
  wifiManager.addParameter(&mqttUsernameParam);
  wifiManager.addParameter(&mqttPasswordParam);
  wifiManager.addParameter(&mqttTopicParam);
  wifiManager.addParameter(&hostnameParam);

  wifiManager.setAPCallback([](WiFiManager *wifiManager) {
    blinker.attach_ms(500, []() { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); });
  });

  wifiManager.setSaveConfigCallback([]() {
    blinker.detach();
    digitalWrite(LED_BUILTIN, HIGH);
    Serial << "Configuration saved!" << endl;
    configurationSaved = true;
  });

  wifiManager.setConfigPortalTimeout(180);

  if (!wifiManager.autoConnect("ESP-BT-MQTT-Setup")) {
    Serial << "Timeout while trying to establish WiFi connection. Rebooting.." << endl;
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  if (configurationSaved) {
    Serial << "Saving configuration to file." << endl;
    config.mqttServer   = String(mqttServerParam.getValue());
    config.mqttPort     = atol(mqttPortParam.getValue());
    config.mqttUsername = String(mqttUsernameParam.getValue());
    config.mqttPassword = String(mqttPasswordParam.getValue());
    config.mqttTopic    = String(mqttTopicParam.getValue());
    config.hostname     = String(hostnameParam.getValue());
    saveConfigToFile();
  }

  Serial << endl
         << "Connected to WiFi: " << WiFi.SSID() << endl
         << "IP address: " << WiFi.localIP() << endl;
}
