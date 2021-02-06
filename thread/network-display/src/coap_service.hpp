#pragma once

#include <functional>
#include <coap_api.h>
#include "coap_helpers.hpp"
#include "common_coap_service.hpp"

namespace coap_service {

  void initialize(const common_request_handler &, const coap_helpers::post_handler &display_post_handler);
}


