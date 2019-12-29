#include <StreamPrint.h>

#include "wifi.hpp"
#include "config.hpp"

bool configurationSaved = false;

void connectWiFi(WiFiManager &wifiManager) {
  Serial << "Connecting to WiFi.." << endl;
  configurationSaved = false;

  WiFiManagerParameter mqttServerParam("mqtt_server", "MQTT server", config.mqttServer.c_str(), 100);
  WiFiManagerParameter mqttPortParam("mqtt_port", "MQTT port", String(config.mqttPort).c_str(), 6);
  WiFiManagerParameter mqttTopicParam("mqtt_topc", "MQTT topic", config.mqttTopic.c_str(), 100);
  WiFiManagerParameter hostnameParam("hostname", "MDNS hostname", config.hostname.c_str(), 100);

  wifiManager.addParameter(&mqttServerParam);
  wifiManager.addParameter(&mqttPortParam);
  wifiManager.addParameter(&mqttTopicParam);
  wifiManager.addParameter(&hostnameParam);

  wifiManager.setSaveConfigCallback([]() {
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
    config.mqttServer = String(mqttServerParam.getValue());
    config.mqttTopic  = String(mqttTopicParam.getValue());
    config.mqttPort   = atol(mqttPortParam.getValue());
    config.hostname   = String(hostnameParam.getValue());
    saveConfigToFile();
  }

  Serial << endl
         << "Connected to WiFi: " << WiFi.SSID() << endl
         << "IP address: " << WiFi.localIP() << endl;
}
