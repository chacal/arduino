#pragma once

#include <functional>
#include <coap_api.h>
#include "coap_helpers.hpp"

namespace coap_service {

  struct request_handler {
    request_handler &operator=(const request_handler &other) = default;

    coap_helpers::post_handler on_display_image_post;
    coap_helpers::get_handler  on_settings_get;
    coap_helpers::post_handler on_settings_post;
    coap_helpers::get_handler  on_state_get;
  };

  void initialize(const request_handler &);
}


