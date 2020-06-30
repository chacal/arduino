#include <cstring>
#include <app_error.h>
#include <coap_api.h>
#include <coap_message.h>
#include <coap_block.h>
#include <nrf_log.h>

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

  coap_block_opt_block1_t get_opt_block1(coap_message_t *p_request) {
    uint8_t option_index;
    coap_message_opt_index_get(&option_index, p_request, COAP_OPT_BLOCK1);
    coap_option_t *p_block1_opt = &p_request->options[option_index];

    // Decode option value into a uint32_t.
    uint32_t block1_opt_value_raw;
    APP_ERROR_CHECK(coap_opt_uint_decode(&block1_opt_value_raw, p_block1_opt->length, p_block1_opt->p_data));

    // Decode uint32_t value into block1 option structure.
    coap_block_opt_block1_t block1_request;
    APP_ERROR_CHECK(coap_block_opt_block1_decode(&block1_request, block1_opt_value_raw));

    return block1_request;
  }

  uint32_t get_encoded_block1_response_opt(const coap_block_opt_block1_t &block1_request_opt) {
    coap_block_opt_block1_t block1_response = {
      .more   = block1_request_opt.more,
      .size   = block1_request_opt.size,
      .number = block1_request_opt.number
    };

    uint32_t block1_response_encoded;
    APP_ERROR_CHECK(coap_block_opt_block1_encode(&block1_response_encoded, &block1_response));
    return block1_response_encoded;
  }

  std::vector<uint8_t> msg_buf;

  void handle_block1_post(coap_message_t *p_request, const coap_service::post_handler &handler) {
    auto block1_request_opt = get_opt_block1(p_request);
    NRF_LOG_DEBUG("Block1: payload: %dB, num: %d, more: %d", p_request->payload_len, block1_request_opt.number, block1_request_opt.more)

    if (block1_request_opt.number == 0) {
      msg_buf.clear();
    }
    msg_buf.insert(msg_buf.end(), &p_request->p_payload[0], &p_request->p_payload[p_request->payload_len]);

    auto     response_status             = block1_request_opt.more == 1 ? COAP_CODE_231_CONTINUE : COAP_CODE_204_CHANGED;
    auto     p_response                  = create_response_for(p_request, response_status);
    uint32_t block1_response_opt_encoded = get_encoded_block1_response_opt(block1_request_opt);
    APP_ERROR_CHECK(coap_message_opt_uint_add(p_response, COAP_OPT_BLOCK1, block1_response_opt_encoded));

    send_and_delete(p_response);

    if (block1_request_opt.more == COAP_BLOCK_OPT_BLOCK_MORE_BIT_UNSET) {
      NRF_LOG_DEBUG("Block1 complete: total payload: %dB", msg_buf.size())
      handler({msg_buf.data(), (uint16_t) msg_buf.size()});
      msg_buf.clear();
    }
  }

  void handle_post(coap_resource_t *p_resource, coap_message_t *p_request, const coap_service::post_handler &handler,
                   uint32_t content_type_mask) {
    if (has_content_type(p_request, content_type_mask)) {
      if (coap_message_opt_present(p_request, COAP_OPT_BLOCK1) == NRF_SUCCESS) {
        handle_block1_post(p_request, handler);
      } else {
        // Create a temporary copy of the payload in order to respond and release the request before actually processing it
        msg_buf.clear();
        msg_buf.insert(msg_buf.end(), &p_request->p_payload[0], &p_request->p_payload[p_request->payload_len]);

        // Respond
        auto p_response = create_response_for(p_request, COAP_CODE_204_CHANGED);
        send_and_delete(p_response);

        // Run handler
        handler({msg_buf.data(), (uint16_t) msg_buf.size()});
      }
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