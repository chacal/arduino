#pragma once

#include <variant>
#include <nrf_log.h>
#include <coap_helpers.hpp>
#include "timer.hpp"
#include "states/context.hpp"
#include "ArduinoJson-v6.13.0.hpp"

#define MGMT_DISCOVERY_PERIOD  std::chrono::seconds(60)
#define DISCOVERY_SERVER_ADDR  "ff03::1"
#define DISCOVERY_SERVER_PORT  8000
#define DISCOVERY_SERVER_PATH  "discover"
#define CONFIGURE_DELAY        std::chrono::seconds(3)

using namespace fsm;

namespace states {

  template<typename M, typename CONFIGURE>
  struct discover : M::Base {
    struct coap_timer_ticked {
    };
    struct mgmt_server_discovery_timer_ticked {
    };
    struct mgmt_server_set {
    };
    struct configure_delay_elapsed {
    };

    virtual void enter(typename M::Context &context) {
      mgmt_server_discovery_timer.start(&context);
      coap_tick_timer.start(&context);
      discover_mgmt_server(context);
    }

    virtual void react(const coap_timer_ticked &event, typename M::Control &control, typename M::Context &context) {
      coap_time_tick();
    }

    virtual void react(const mgmt_server_discovery_timer_ticked &event, typename M::Control &control, typename M::Context &context) {
      discover_mgmt_server(context);
    }

    virtual void react(const mgmt_server_set &event, typename M::Control &control, typename M::Context &context) {
      NRF_LOG_INFO("Discovered mgmt server! %s", context.mgmt_server.c_str())
      mgmt_server_discovery_timer.stop();
      coap_tick_timer.stop();
      configure_delay_timer.start(&context);
    }

    virtual void react(const configure_delay_elapsed &event, typename M::Control &control, typename M::Context &context) {
      control.template changeTo<CONFIGURE>();
    }

    using M::Base::react;


  private:
    milliseconds   coap_tick_period = std::chrono::seconds(1);
    periodic_timer coap_tick_timer{coap_tick_period, [](void *ctx) {
      static_cast<typename M::Context *>(ctx)->react(coap_timer_ticked{});
    }};
    periodic_timer mgmt_server_discovery_timer{MGMT_DISCOVERY_PERIOD, [](void *ctx) {
      static_cast<typename M::Context *>(ctx)->react(mgmt_server_discovery_timer_ticked{});
    }};
    oneshot_timer  configure_delay_timer{CONFIGURE_DELAY, [](void *ctx) {
      static_cast<typename M::Context *>(ctx)->react(configure_delay_elapsed{});
    }};

    void discover_mgmt_server(typename M::Context &context) {
      NRF_LOG_INFO("Discovering mgmt server..");

      auto responseHandler = [](uint32_t status, void *p_arg, coap_message_t *p_message) {
        if (status != NRF_SUCCESS) {
          NRF_LOG_ERROR("Error status from discover response. Status: %d", status)
        } else if (p_message->header.code != COAP_CODE_205_CONTENT) {
          NRF_LOG_ERROR("Unexpected discovery server response code: %d", p_message->header.code)
        } else {
          ArduinoJson::StaticJsonDocument<500> doc;
          ArduinoJson::DeserializationError    error = deserializeJson(doc, p_message->p_payload);

          if (error) {
            NRF_LOG_ERROR("Parsing discovery response JSON failed!");
            NRF_LOG_HEXDUMP_ERROR(p_message->p_payload, p_message->payload_len)
            return;
          }

          auto ctx = static_cast<typename M::Context *>(p_arg);
          ctx->mgmt_server = doc["mgmtServer"].as<char *>();
          ctx->react(mgmt_server_set{});
        }
      };

      coap_helpers::get(DISCOVERY_SERVER_ADDR, DISCOVERY_SERVER_PORT, DISCOVERY_SERVER_PATH, responseHandler, &context);
    }
  };
}
