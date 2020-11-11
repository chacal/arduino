#pragma once

#include <variant>
#include <nrf_log.h>
#include <settings.hpp>
#include <coap_helpers.hpp>
#include "timer.hpp"
#include "common.hpp"
#include "coap_service.hpp"
#include "ArduinoJson-v6.13.0.hpp"
#include "configure.hpp"

#define MGMT_DISCOVERY_PERIOD  std::chrono::seconds(60)
#define DISCOVERY_SERVER_ADDR  "ff03::1"
#define DISCOVERY_SERVER_PORT  8000
#define DISCOVERY_SERVER_PATH  "discover"
#define CONFIGURE_DELAY        std::chrono::seconds(3)

using namespace fsm;

namespace states {

  struct discover : Base {
    struct mgmt_server_discovery_timer_ticked {
    };
    struct mgmt_server_set {
    };
    struct configure_delay_elapsed {
    };

    virtual void enter(Context &context) {
      mgmt_server_discovery_timer.start(&context);
      discover_mgmt_server(context);
    }

    virtual void react(const mgmt_server_discovery_timer_ticked &event, Control &control, Context &context) {
      discover_mgmt_server(context);
    }

    virtual void react(const mgmt_server_set &event, Control &control, Context &context) {
      NRF_LOG_INFO("Discovered mgmt server! %s", context.mgmt_server_address.c_str())
      mgmt_server_discovery_timer.stop();
      configure_delay_timer.start(&context);
    }

    virtual void react(const configure_delay_elapsed &event, Control &control, Context &context) {
      control.changeTo<configure>();
    }

    using Base::react;


  private:
    periodic_timer mgmt_server_discovery_timer{MGMT_DISCOVERY_PERIOD, [](void *ctx) {
      static_cast<Context *>(ctx)->react(mgmt_server_discovery_timer_ticked{});
    }};
    oneshot_timer  configure_delay_timer{CONFIGURE_DELAY, [](void *ctx) {
      static_cast<Context *>(ctx)->react(configure_delay_elapsed{});
    }};

    void discover_mgmt_server(Context &context) {
      NRF_LOG_INFO("Discovering mgmt server..");

      auto responseHandler = [](uint32_t status, void *p_arg, coap_message_t *p_message) {
        if (status != NRF_SUCCESS) {
          NRF_LOG_ERROR("Error status from discover response. Status: %d", status)
          return;
        }

        ArduinoJson::StaticJsonDocument<500> doc;
        ArduinoJson::DeserializationError    error = deserializeJson(doc, p_message->p_payload);

        if (error) {
          NRF_LOG_ERROR("Parsing discovery response JSON failed!");
          NRF_LOG_HEXDUMP_ERROR(p_message->p_payload, p_message->payload_len)
          return;
        }

        auto ctx = static_cast<Context *>(p_arg);
        ctx->mgmt_server_address = doc["mgmtServer"].as<char *>();
        ctx->react(mgmt_server_set{});
      };

      coap_helpers::get(DISCOVERY_SERVER_ADDR, DISCOVERY_SERVER_PORT, DISCOVERY_SERVER_PATH, responseHandler, &context);
    }
  };
}
