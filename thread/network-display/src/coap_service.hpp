#pragma once

#include <functional>
#include <coap_api.h>

namespace coap_service {

  struct post_data {
    const uint8_t *data;
    uint16_t      len;
  };

  using post_handler = std::function<void(const post_data &)>;
  using get_handler = std::function<std::string()>;

  struct request_handler {
    request_handler &operator=(const request_handler &other) = default;

    post_handler on_display_image_post;
    get_handler on_settings_get;
    post_handler on_settings_post;
    get_handler on_status_get;
  };

  void initialize(const request_handler &);
}


