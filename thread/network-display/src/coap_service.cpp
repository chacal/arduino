#include <string>
#include <nrf_log.h>
#include <app_error.h>
#include <coap_message.h>
#include "coap_service.hpp"
#include "coap_helpers.hpp"

namespace coap_service {

  static request_handler m_request_handler;
  static coap_resource_t m_api_resource      = {};
  static coap_resource_t m_display_resource  = {};
  static coap_resource_t m_status_resource   = {};
  static coap_resource_t m_settings_resource = {};


  static void on_display_post(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_helpers::handle_json_post(p_resource, p_request, m_request_handler.on_display_post);
  }

  static void on_status_get(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_helpers::handle_json_get(p_resource, p_request, m_request_handler.on_status_get);
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

  void initialize_settings_resource() {
    coap_helpers::create_resource(m_settings_resource, "settings");

    m_settings_resource.permission      = COAP_PERM_POST | COAP_PERM_GET;
    m_settings_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
    m_settings_resource.callback        = on_settings_request;
  }

  void initialize(const request_handler &handler) {
    m_request_handler = handler;
    initialize_api_resource();
    initialize_display_resource();
    initialize_status_resource();
    initialize_settings_resource();

    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_display_resource));
    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_status_resource));
    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_settings_resource));
    coap_helpers::add_resource_to_root(m_api_resource);
  }

}