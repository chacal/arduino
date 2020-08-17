#include <ESPAsyncWebServer.h>
#include <FS.h>


static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

void webServerInit() {
  SPIFFS.begin();
  server.addHandler(&ws);

  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void webServerBroadcastWs(const String &msg) {
  ws.textAll(msg);
}