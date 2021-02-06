#pragma once

#include <app_error.h>
#include <coap_helpers.hpp>

namespace coap_service {
  struct common_request_handler {
    common_request_handler &operator=(const common_request_handler &other) = default;

    coap_helpers::get_handler  on_settings_get;
    coap_helpers::post_handler on_settings_post;
    coap_helpers::get_handler  on_state_get;
  };

  extern coap_resource_t m_api_resource;

  void initialize_common_resources(const common_request_handler &handler);
}