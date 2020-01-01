#include <optional>
#include "cmd_protocol.hpp"

#define ARDUINOJSON_EMBEDDED_MODE   1
#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v6.13.0.hpp"


using namespace ArduinoJson;


namespace commands {
  static std::optional<display_command> parse(const JsonObject &obj) {
    auto cmd_type = obj["c"].as<char *>()[0];
    switch (cmd_type) {
      case 's':
        return {str_cmd{
          obj["i"].as<uint8_t>(),
          point(obj["x"].as<uint16_t>(), obj["y"].as<uint16_t>()),
          font_size(obj["font"].as<uint8_t>()),
          obj["msg"].as<char *>()
        }};
      case 'l':
        return {line_cmd{
          obj["i"].as<uint8_t>(),
          point(obj["x1"].as<uint16_t>(), obj["y1"].as<uint16_t>()),
          point(obj["x2"].as<uint16_t>(), obj["y2"].as<uint16_t>()),
        }};
      case 'c':
        return {clear_cmd{}};
      default:
        NRF_LOG_ERROR("Invalid command type: %c", cmd_type);
        return std::nullopt;
    }
  }

  const std::unique_ptr<display_command_seq> parse(const coap_service::post_data &data) {
    StaticJsonDocument<JSON_PARSE_BUFFER_SIZE> doc;
    DeserializationError                       error = deserializeJson(doc, data.data);

    if (error) {
      NRF_LOG_ERROR("Parsing JSON input failed!");
      return std::unique_ptr<display_command_seq>();  // Return empty pointer
    }

    if (!doc.is<JsonArray>()) {
      NRF_LOG_ERROR("Expecting JSON array of commands!");
      return std::unique_ptr<display_command_seq>();  // Return empty pointer
    }

    JsonArray arr = doc.as<JsonArray>();
    auto      ret = std::make_unique<display_command_seq>();

    for (JsonObject elem : arr) {
      if (auto cmd = parse(elem)) {
        ret->push_back(cmd.value());
      }
    }

    return ret;
  }
}