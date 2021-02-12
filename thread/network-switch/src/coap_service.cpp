#include "coap_service.hpp"
#include "coap_helpers.hpp"

namespace coap_service {
  static coap_helpers::post_handler m_switch_post_handler;
  static coap_resource_t            m_switch_resource = {};

  static void on_switch_post(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_helpers::handle_json_post(p_resource, p_request, m_switch_post_handler);
  }

  void initialize_switch_resource() {
    coap_helpers::create_resource(m_switch_resource, "switch");
    m_switch_resource.permission      = COAP_PERM_POST;
    m_switch_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
    m_switch_resource.callback        = on_switch_post;
  }


  void initialize(const common_request_handler &handler, const coap_helpers::post_handler &on_switch_post) {
    m_switch_post_handler = on_switch_post;
    initialize_common_resources(handler);
    initialize_switch_resource();

    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_switch_resource));
    coap_helpers::add_resource_to_root(m_api_resource);
  }

}