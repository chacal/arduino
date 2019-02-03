#pragma once

#include <string>
#include <coap_api.h>

namespace coap_helpers {
  void add_resource_to_root(coap_resource_t &resource);
  void create_resource(coap_resource_t &resource, const std::string &name);
  bool has_content_type(coap_message_t *p_request, uint32_t coap_content_type_mask);

  coap_message_t * create_response_for(coap_message_t *p_request, coap_msg_code_t response_code);
  void send_and_delete(coap_message_t *p_response);
}