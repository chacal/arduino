#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include "LittleFS.h"
#include "config.hpp"


static AsyncWebServer        server(80);
static AsyncWebSocket        ws("/ws");
static std::function<void()> onConfigUpdated;

void onGetConfig(AsyncWebServerRequest *request) {
  auto response = request->beginResponseStream("application/json");
  serializeJson(getConfigAsJson(), *response);
  request->send(response);
}

void onPostConfig(AsyncWebServerRequest *request, const JsonVariant &json) {
  updateConfigFromJson(json);
  saveConfigToFile();
  request->send(204);
  onConfigUpdated();
}

void webServerInit(const std::function<void()> &configUpdated) {
  onConfigUpdated = configUpdated;

  LittleFS.begin();
  server.on("/config", HTTP_GET, onGetConfig);
  server.addHandler(new AsyncCallbackJsonWebHandler("/config", onPostConfig));

  server.addHandler(&ws);

  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void webServerBroadcastWs(const String &msg) {
  ws.textAll(msg);
}