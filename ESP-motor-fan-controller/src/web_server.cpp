#include <ESPAsyncWebServer.h>
#include "LittleFS.h"


static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void webServerInit() {
  LittleFS.begin();
  server.addHandler(&ws);

  server.serveStatic("/", LittleFS, "/");
  server.begin();
}

void webServerBroadcastWs(const String &msg) {
  ws.textAll(msg);
}