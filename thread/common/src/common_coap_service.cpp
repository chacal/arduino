#include <app_error.h>
#include "common_coap_service.hpp"


namespace coap_service {
  static common_request_handler m_request_handler;

  coap_resource_t        m_api_resource      = {};
  static coap_resource_t m_state_resource    = {};
  static coap_resource_t m_settings_resource = {};

  static void on_state_get(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_helpers::handle_json_get(p_resource, p_request, m_request_handler.on_state_get);
  }

  static void on_settings_request(coap_resource_t *p_resource, coap_message_t *p_request) {
    if (p_request->header.code == COAP_CODE_GET) {
      coap_helpers::handle_json_get(p_resource, p_request, m_request_handler.on_settings_get);
    } else {
      coap_helpers::handle_json_post(p_resource, p_request, m_request_handler.on_settings_post);
    }
  }

  void initialize_api_resource() {
    coap_helpers::create_resource(m_api_resource, "api");
  }

  void initialize_state_resource() {
    coap_helpers::create_resource(m_state_resource, "state");

    m_state_resource.permission      = COAP_PERM_GET;
    m_state_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
    m_state_resource.callback        = on_state_get;
  }

  void initialize_settings_resource() {
    coap_helpers::create_resource(m_settings_resource, "settings");

    m_settings_resource.permission      = COAP_PERM_POST | COAP_PERM_GET;
    m_settings_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
    m_settings_resource.callback        = on_settings_request;
  }

  void initialize_common_resources(const common_request_handler &handler) {
    m_request_handler = handler;
    initialize_api_resource();
    initialize_state_resource();
    initialize_settings_resource();
    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_state_resource));
    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_settings_resource));
  }
}