#pragma once

#include <nrf_log.h>
#include <settings.hpp>
#include "periodic_timer.hpp"
#include "common.hpp"
#include "coap_service.hpp"
#include "cmd_protocol.hpp"
#include "display_command_handler.hpp"

#define COAP_TICK_PERIOD      std::chrono::seconds(1)

using namespace fsm;

namespace states {

  struct connected : Base {
    struct coap_timer_ticked {};

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected");
      coap_tick_timer.start(&context);
      context.disp.on();

      auto on_display_post = [&](auto coap_data) { handle_display_post(coap_data, context); };
      coap_service::initialize({
                                   on_display_post,
                                   &settings::get_as_json,
                                   &settings::update,
                                   &util::get_status_json,
                               });
    }

    virtual void react(const coap_timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    using Base::react;


  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(coap_timer_ticked{}); }};

    void handle_display_post(const coap_service::post_data &coap_data, Context &context) {
      NRF_LOG_DEBUG("Got %d bytes of data", coap_data.len)
      auto cmd_seq = commands::parse(coap_data);
      if (cmd_seq) {
        NRF_LOG_DEBUG("Received %d commands", cmd_seq->size())
        for (auto &cmd : *cmd_seq) {
          std::visit(display_command_handler{context.display_list}, cmd);
        }
        context.display_list.render(context.disp);
      }
    }
  };
}

