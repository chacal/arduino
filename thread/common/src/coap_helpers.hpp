#pragma once

#include <string>
#include <functional>
#include <coap_api.h>

namespace coap_helpers {
  struct post_data {
    const uint8_t *data;
    uint16_t      len;
  };

  using post_handler = std::function<void(const post_data &)>;
  using get_handler = std::function<std::string()>;

  void add_resource_to_root(coap_resource_t &resource);

  void create_resource(coap_resource_t &resource, const std::string &name);

  void handle_json_post(coap_resource_t *p_resource, coap_message_t *p_request, const post_handler &handler);

  void handle_binary_post(coap_resource_t *p_resource, coap_message_t *p_request, const post_handler &handler);

  void handle_json_get(coap_resource_t *p_resource, coap_message_t *p_request, const get_handler &handler);

  void get(const std::string &addr, uint16_t port, const std::string &path, coap_response_callback_t response_callback, void *ctx);

  void post_json(const std::string &addr, uint16_t port, const std::string &path, const std::string &payload,
                 coap_response_callback_t response_callback, void *ctx);
}