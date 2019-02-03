#include <string>
#include <nrf_log.h>
#include <app_error.h>
#include <coap_message.h>
#include "coap_service.hpp"
#include "coap_helpers.hpp"


static void on_display_post(coap_resource_t *p_resource, coap_message_t *p_request) {
  coap_message_t *p_response;

  if(coap_helpers::has_content_type(p_request, COAP_CT_MASK_APP_JSON)) {
    NRF_LOG_INFO("Got %d bytes of POST payload", p_request->payload_len)
    p_response = coap_helpers::create_response_for(p_request, COAP_CODE_204_CHANGED);
  } else {
    p_response = coap_helpers::create_response_for(p_request, COAP_CODE_415_UNSUPPORTED_CONTENT_FORMAT);
  }

  coap_helpers::send_and_delete(p_response);
}

void coap_service::initialize_api_resource() {
  coap_helpers::create_resource(api_resource, "api");
}

void coap_service::initialize_display_resource() {
  coap_helpers::create_resource(display_resource, "display");

  display_resource.permission      = COAP_PERM_POST ;
  display_resource.ct_support_mask = COAP_CT_MASK_APP_JSON;
  display_resource.callback        = on_display_post;
}

void coap_service::initialize() {
  initialize_api_resource();
  initialize_display_resource();

  APP_ERROR_CHECK(coap_resource_child_add(&api_resource, &display_resource));
  coap_helpers::add_resource_to_root(api_resource);
}
