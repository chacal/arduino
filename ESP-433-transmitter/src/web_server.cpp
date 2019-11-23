#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include "transmitter.hpp"


static AsyncWebServer server(80);

bool validate_url(const String &url) {
  return url.length() == 15 && isAlpha(url.charAt(10)) &&
         isDigit(url.charAt(12)) && isDigit(url.charAt(14));
}

TxTarget target_from_url(const String &url) {
  char family = url.charAt(10);
  uint32_t group = url.charAt(12) - '0';
  uint32_t device = url.charAt(14) - '0';
  return {family, group, device};
}

void on_post_transmit(AsyncWebServerRequest *request, const JsonVariant &json) {
  auto url = request->url();

  if (validate_url(url) && json.containsKey("state") && json["state"].is<bool>()) {
    auto target = target_from_url(url);
    tx_submit({target, json["state"]});
    request->send(200);
  } else {
    request->send(400, "application/json", R"({"error": "Invalid request"})");
  }
}

void on_get_status(AsyncWebServerRequest *request) {
  auto url = request->url();

  if (validate_url(url)) {
    auto state = tx_get_state(target_from_url(url));

    auto response = request->beginResponseStream("application/json");
    response->setCode(200);
    response->printf(R"({"state": %s})", state ? "true" : "false");
    request->send(response);
  } else {
    request->send(404, "application/json", R"({"error": "Invalid request"})");
  }
}

void web_server_init() {
  server.on("/transmit", HTTP_GET, on_get_status);
  server.addHandler(new AsyncCallbackJsonWebHandler("/transmit", on_post_transmit));
  server.begin();
}
