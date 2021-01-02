#pragma once

#include <nrf_log.h>
#include <hfsm/machine_single.hpp>
#include "common.hpp"
#include "connected.hpp"

#define CONFIG_TIMER_PERIOD   std::chrono::seconds(60)

using namespace fsm;

namespace states {

  struct configure : Base {
    struct config_timer_elapsed {
    };
    struct config_set {
    };
    struct coap_timer_ticked {
    };

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Configuring..");
      config_timer.start(&context);
      coap_tick_timer.start(&context);
      get_configuration(context);
    }

    virtual void leave(Context &context) {
      config_timer.stop();
      coap_tick_timer.stop();
    }

    virtual void react(const config_set &event, Control &control, Context &context) {
      NRF_LOG_INFO("Configured:")
      NRF_LOG_INFO("%s", settings::get_as_json().c_str())
      control.changeTo<connected>();
    }

    virtual void react(const coap_timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    virtual void react(const config_timer_elapsed &event, Control &control, Context &context) {
      get_configuration(context);
    }

    using Base::react;

  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(coap_timer_ticked{}); }};
    periodic_timer config_timer{CONFIG_TIMER_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(config_timer_elapsed{}); }};

    void get_configuration(Context &context) {
      auto id = util::get_device_id();
      NRF_LOG_INFO("Getting configuration from %s. Device ID: %s", settings::m_mgmt_server.c_str(), id.c_str());

      auto responseHandler = [](uint32_t status, void *p_arg, coap_message_t *p_message) {
        if (status != NRF_SUCCESS) {
          NRF_LOG_ERROR("Error status from config response. Status: %d", status)
        } else if (p_message->header.code != COAP_CODE_205_CONTENT) {
          NRF_LOG_ERROR("Unexpected management server response code: %d", p_message->header.code)
        } else {
          settings::update(p_message->p_payload, p_message->payload_len);
          auto ctx = static_cast<Context *>(p_arg);
          if (settings::m_display_type) {
            ctx->disp = settings::createDisplay(*settings::m_display_type);
          }
          ctx->react(config_set{});
        }
      };

      coap_helpers::get(settings::m_mgmt_server, MGMT_SERVER_PORT, "v1/defaults/" + id, responseHandler, &context);
    }
  };
}
