#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include "transmitter.hpp"


static AsyncWebServer server(80);

void respondWithMissingField(const String &fieldName, AsyncWebServerRequest *request) {
  auto res = String(R"({"error": "Missing or invalid field: ')") + fieldName + R"('"})";
  request->send(400, "application/json", res);
}

void on_post_transmit(AsyncWebServerRequest *request, const JsonVariant &json) {
  if (!json.containsKey("family") || !json["family"].is<char *>() || json["family"].as<String>().length() != 1) {
    respondWithMissingField("family", request);
  } else if (!json.containsKey("group") || !json["group"].is<unsigned int>()) {
    respondWithMissingField("group", request);
  } else if (!json.containsKey("device") || !json["device"].is<unsigned int>()) {
    respondWithMissingField("device", request);
  } else if (!json.containsKey("state") || !json["state"].is<bool>()) {
    respondWithMissingField("state", request);
  } else {
    String family = json["family"];
    uint32_t group = json["group"];
    uint32_t device = json["device"];
    bool state = json["state"];

    tx_submit({family.charAt(0), group, device, state});
    request->send(204);
  }
}

void web_server_init() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(200, "text/plain", String(ESP.getFreeHeap()));
  });
  server.addHandler(new AsyncCallbackJsonWebHandler("/transmit", on_post_transmit));
  server.begin();
}
