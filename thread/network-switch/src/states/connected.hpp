#pragma once

#include <variant>
#include <nrf_log.h>
#include <coap_helpers.hpp>
#include "states/context.hpp"
#include "gzip.hpp"

#define COAP_TICK_PERIOD       std::chrono::seconds(1)
#define MGMT_SERVER_PORT       5683

using namespace fsm;

namespace states {

  struct connected : Base {
    struct coap_timer_ticked {
    };

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected!");
      coap_tick_timer.start(&context);
      post_state_to_mgmt_server(context.mgmt_server);
    }

    virtual void react(const coap_timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    using Base::react;


  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(coap_timer_ticked{}); }};

    void post_state_to_mgmt_server(const std::string &mgmt_server) const {
      // TODO: Get instance from settings
      auto state = util::get_state_json("0000");

      NRF_LOG_INFO("Posting state to mgmt server:")
      NRF_LOG_INFO("%s", state.c_str())

      coap_helpers::post_json(
        mgmt_server, MGMT_SERVER_PORT, "v1/state/" + util::get_device_id(),
        state,
        [](uint32_t status, void *p_arg, coap_message_t *p_message) {
          NRF_LOG_INFO("State post status: %d", status)
          NRF_LOG_INFO("State post reponse code: %d", p_message->header.code)
        }, nullptr);
    }
  };
}
