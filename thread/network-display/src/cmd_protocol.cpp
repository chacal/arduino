#include <optional>
#include "cmd_protocol.hpp"

#define ARDUINOJSON_EMBEDDED_MODE   1
#define JSON_PARSE_BUFFER_SIZE      2000

#include "ArduinoJson-v5.13.4.hpp"


using namespace ArduinoJson;


namespace commands {
  static std::optional<display_command> parse(const JsonObject &obj) {
    auto cmd_type = obj.get<char *>("c")[0];
    switch (cmd_type) {
      case 's':
        return {str_cmd{
            obj.get<uint8_t>("i"),
            point(obj.get<uint8_t>("x"), obj.get<uint8_t>("y")),
            font_size(obj.get<uint8_t>("font")),
            obj.get<char *>("msg")
        }};
      case 'l':
        return {line_cmd{
            obj.get<uint8_t>("i"),
            point(obj.get<uint8_t>("x1"), obj.get<uint8_t>("y1")),
            point(obj.get<uint8_t>("x2"), obj.get<uint8_t>("y2")),
        }};
      case 'c':
        return {clear_cmd{}};
      default:
        NRF_LOG_ERROR("Invalid command type: %c", cmd_type);
        return std::nullopt;
    }
  }

  const std::unique_ptr<display_command_seq> parse(const coap_service::post_data &data) {
    StaticJsonBuffer<JSON_PARSE_BUFFER_SIZE> doc;
    JsonArray                                &arr = doc.parseArray(data.data);

    if (!arr.success()) {
      NRF_LOG_INFO("Parsing JSON input failed!");
      return std::unique_ptr<display_command_seq>();  // Return empty pointer
    }

    auto ret = std::make_unique<display_command_seq>();

    for (JsonObject &elem : arr) {
      if (auto cmd = parse(elem)) {
        ret->push_back(cmd.value());
      }
    }

    return ret;
  }
}