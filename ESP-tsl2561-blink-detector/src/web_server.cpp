#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <AsyncJson.h>

#include "config.hpp"

static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void on_get_config(AsyncWebServerRequest *request) {
  auto response = request->beginResponseStream("application/json");
  serializeJson(get_config_as_json(), *response);
  request->send(response);
}

void on_post_config(AsyncWebServerRequest *request, const JsonVariant &json) {
  update_config_from_json(json);
  request->send(204);
}

void web_server_init() {
  SPIFFS.begin();
  server.on("/config", HTTP_GET, on_get_config);
  server.addHandler(new AsyncCallbackJsonWebHandler("/config", on_post_config));
  server.addHandler(&ws);

  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void web_server_broadcast_ws(const String &msg) {
  ws.textAll(msg);
}