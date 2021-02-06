#include "coap_service.hpp"
#include "coap_helpers.hpp"

namespace coap_service {

  void initialize(const common_request_handler &handler) {
    initialize_common_resources(handler);
    coap_helpers::add_resource_to_root(m_api_resource);
  }

}