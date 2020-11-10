#pragma once

#include <string>
#include <coap_api.h>
#include "coap_service.hpp"

namespace coap_helpers {
  void add_resource_to_root(coap_resource_t &resource);
  void create_resource(coap_resource_t &resource, const std::string &name);

  void handle_json_post(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::post_handler &handler);
  void handle_binary_post(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::post_handler &handler);
  void handle_json_get(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::get_handler &handler);

  void get(const std::string &addr, uint16_t port, const std::string &path, coap_response_callback_t response_callback, void *ctx);
}