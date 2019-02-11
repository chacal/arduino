#include <string>
#include <nrf_log.h>
#include <app_error.h>
#include <coap_message.h>
#include "coap_service.hpp"
#include "coap_helpers.hpp"

namespace coap_service {

  static request_handler m_request_handler;
  static coap_resource_t m_api_resource     = {};
  static coap_resource_t m_display_resource = {};
  static coap_resource_t m_status_resource  = {};

  static void on_display_post(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_message_t *p_response;

    if (coap_helpers::has_content_type(p_request, COAP_CT_MASK_APP_JSON)) {
      m_request_handler.on_display_post({p_request->p_payload, p_request->payload_len});
      p_response = coap_helpers::create_response_for(p_request, COAP_CODE_204_CHANGED);
    } else {
      p_response = coap_helpers::create_response_for(p_request, COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT);
    }

    coap_helpers::send_and_delete(p_response);
  }

  static void on_status_get(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_message_t      *p_response;
    coap_content_type_t content_type;
    uint32_t            err_code = coap_message_ct_match_select(&content_type, p_request, p_resource);

    if (err_code == NRF_SUCCESS && content_type == COAP_CT_APP_JSON) {
      p_response = coap_helpers::create_response_for(p_request, COAP_CODE_205_CONTENT);
      auto response_content = m_request_handler.on_status_get();
      coap_message_payload_set(p_response, (void *) (response_content.c_str()), response_content.length());
    } else {
      p_response = coap_helpers::create_response_for(p_request, COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT);
    }

    coap_helpers::send_and_delete(p_response);
  }

  void initialize_api_resource() {
    coap_helpers::create_resource(m_api_resource, "api");
  }

  void initialize_display_resource() {
    coap_helpers::create_resource(m_display_resource, "display");

    m_display_resource.permission      = COAP_PERM_POST;
    m_display_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
    m_display_resource.callback        = on_display_post;
  }

  void initialize_status_resource() {
    coap_helpers::create_resource(m_status_resource, "status");

    m_status_resource.permission      = COAP_PERM_GET;
    m_status_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
    m_status_resource.callback        = on_status_get;
  }

  void initialize(const request_handler &handler) {
    m_request_handler = handler;
    initialize_api_resource();
    initialize_display_resource();
    initialize_status_resource();

    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_display_resource));
    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_status_resource));
    coap_helpers::add_resource_to_root(m_api_resource);
  }

}