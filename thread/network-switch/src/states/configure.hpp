#pragma once

#include <nrf_log.h>
#include <hfsm/machine_single.hpp>
#include "states/context.hpp"
#include "states/base_configure.hpp"
#include "connected.hpp"
#include "settings.hpp"

using namespace fsm;

namespace states {
  template<typename M>
  struct configure : base_configure<M> {
    struct config_set {
    };

    virtual void react(const config_set &event, typename M::Control &control, typename M::Context &context) {
      NRF_LOG_INFO("Configured:")
      NRF_LOG_INFO("%s", settings::get_as_json().c_str())
      control.template changeTo<connected>();
    }

    using Base::react;

    virtual void get_configuration(typename M::Context &context) {
      auto id = util::get_device_id();
      NRF_LOG_INFO("Getting configuration from %s. Device ID: %s", context.mgmt_server.c_str(), id.c_str());

      auto responseHandler = [](uint32_t status, void *p_arg, coap_message_t *p_message) {
        if (status != NRF_SUCCESS) {
          NRF_LOG_ERROR("Error status from config response. Status: %d", status)
        } else if (p_message->header.code != COAP_CODE_205_CONTENT) {
          NRF_LOG_ERROR("Unexpected management server response code: %d", p_message->header.code)
        } else {
          settings::update(p_message->p_payload, p_message->payload_len);
          auto ctx = static_cast<typename M::Context *>(p_arg);
          ctx->react(config_set{});
        }
      };

      coap_helpers::get(context.mgmt_server, MGMT_SERVER_PORT, "v1/defaults/" + id, responseHandler, &context);
    }
  };
}
