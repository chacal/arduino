#include <app_error.h>
#include <coap_message.h>
#include "coap_service.hpp"
#include "coap_helpers.hpp"

namespace coap_service {

  static coap_helpers::post_handler m_display_post_handler;
  static coap_resource_t            m_display_image_resource = {};


  static void on_display_image_post(coap_resource_t *p_resource, coap_message_t *p_request) {
    coap_helpers::handle_binary_post(p_resource, p_request, m_display_post_handler);
  }

  void initialize_display_image_resource() {
    coap_helpers::create_resource(m_display_image_resource, "image");

    m_display_image_resource.permission      = COAP_PERM_POST;
    m_display_image_resource.ct_support_mask = COAP_CT_MASK_APP_OCTET_STREAM;
    m_display_image_resource.callback        = on_display_image_post;
  }

  void initialize(const common_request_handler &handler, const coap_helpers::post_handler &display_post_handler) {
    m_display_post_handler = display_post_handler;
    initialize_common_resources(handler);
    initialize_display_image_resource();

    APP_ERROR_CHECK(coap_resource_child_add(&m_api_resource, &m_display_image_resource));
    coap_helpers::add_resource_to_root(m_api_resource);
  }

}