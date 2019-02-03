#pragma once

#include <functional>
#include <coap_api.h>

namespace coap_service {

  struct post_data {
    const uint8_t *data;
    uint16_t      len;
  };

  using post_handler = std::function<void(const post_data &)>;

  struct request_handler {
    request_handler &operator=(const request_handler &other) = default;

    post_handler on_display_post;
  };

  void initialize(const request_handler &);
}


