#pragma once

#include <coap_api.h>

class coap_service {
public:
  void initialize();

private:
  coap_resource_t api_resource = {};
  coap_resource_t display_resource = {};

  void initialize_api_resource();
  void initialize_display_resource();
};