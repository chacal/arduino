#include <cstring>
#include <app_error.h>
#include <coap_api.h>
#include <coap_message.h>

#include "coap_helpers.hpp"

namespace coap_helpers {

  void add_resource_to_root(coap_resource_t &resource) {
    coap_resource_t *root;
    APP_ERROR_CHECK(coap_resource_root_get(&root));
    APP_ERROR_CHECK(coap_resource_child_add(root, &resource));
  }

  void create_resource(coap_resource_t &resource, const std::string &name) {
    APP_ERROR_CHECK(coap_resource_create(&resource, name.c_str()));
    strcpy(resource.name, name.c_str());
  }

  bool has_content_type(coap_message_t *p_request, uint32_t coap_content_type_mask) {
    uint32_t ct_mask;
    coap_message_ct_mask_get(p_request, &ct_mask);
    return coap_content_type_mask == ct_mask;
  }

  coap_message_conf_t response_config_for_request(const coap_message_t *const request, coap_msg_code_t response_code) {
    coap_message_conf_t response_config;
    if (request->header.type == COAP_TYPE_NON) {
      response_config.type = COAP_TYPE_NON;
    } else if (request->header.type == COAP_TYPE_CON) {
      response_config.type = COAP_TYPE_ACK;
    }

    response_config.code             = response_code;
    response_config.id               = request->header.id;
    response_config.port.port_number = DFU_UDP_PORT;
    memcpy(&response_config.token[0], &request->token[0], request->header.token_len);
    response_config.token_len = request->header.token_len;

    return response_config;
  }

  coap_message_t *create_response_for(coap_message_t *p_request, coap_msg_code_t response_code) {
    auto response_config    = response_config_for_request(p_request, response_code);

    coap_message_t *p_response;
    uint32_t       err_code = coap_message_new(&p_response, &response_config);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_remote_addr_set(p_response, &p_request->remote);
    APP_ERROR_CHECK(err_code);

    return p_response;
  }

  void send_and_delete(coap_message_t *p_response) {
    uint32_t msg_handle;
    uint32_t err_code = coap_message_send(&msg_handle, p_response);
    APP_ERROR_CHECK(err_code);

    err_code = coap_message_delete(p_response);
    APP_ERROR_CHECK(err_code);
  }

  void handle_post(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::post_handler &handler,
                   uint32_t content_type_mask) {
    if (has_content_type(p_request, content_type_mask)) {
      // Create a temporary copy of the payload in order to respond and release the request before actually processing it
      uint8_t temp[p_request->payload_len];
      memcpy(temp, p_request->p_payload, p_request->payload_len);
      auto len = p_request->payload_len;

      // Respond
      auto p_response = create_response_for(p_request, COAP_CODE_204_CHANGED);
      send_and_delete(p_response);

      // Run handler
      handler({temp, len});
    } else {
      auto p_response = create_response_for(p_request, COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT);
      send_and_delete(p_response);
    }
  }

  void handle_json_post(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::post_handler &handler) {
    handle_post(p_resource, p_request, handler, COAP_CT_MASK_APP_JSON);
  }

  void handle_binary_post(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::post_handler &handler) {
    handle_post(p_resource, p_request, handler, COAP_CT_MASK_APP_OCTET_STREAM);
  }

  void handle_json_get(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::get_handler &handler) {
    coap_message_t      *p_response;
    coap_content_type_t content_type;
    uint32_t            err_code = coap_message_ct_match_select(&content_type, p_request, p_resource);

    if (err_code == NRF_SUCCESS && content_type == COAP_CT_APP_JSON) {
      p_response = create_response_for(p_request, COAP_CODE_205_CONTENT);
      auto response_content = handler();
      coap_message_payload_set(p_response, (void *) (response_content.c_str()), response_content.length());
    } else {
      p_response = create_response_for(p_request, COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT);
    }

    send_and_delete(p_response);
  }
}