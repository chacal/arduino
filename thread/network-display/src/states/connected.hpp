#pragma once

#include <nrf_log.h>
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

      auto on_coap_post = [&](const coap_service::post_data &coap_data) { context.react(coap_data); };
      coap_service::initialize({
                                   on_coap_post,
                                   &util::get_status_json
                               });
    }

    virtual void react(const coap_timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    virtual void react(const coap_service::post_data &data, Control &control, Context &context) {
      NRF_LOG_DEBUG("Got %d bytes of data", data.len)
      auto cmd_seq = commands::parse(data);
      if (cmd_seq) {
        context.react(*cmd_seq);
      }
    }

    virtual void react(const commands::display_command_seq &commands, Control &control, Context &context) {
      NRF_LOG_DEBUG("Received %d commands", commands.size())
      for (auto &cmd : commands) {
        std::visit(display_command_handler{context.display_list}, cmd);
      }
      context.display_list.render(context.disp);
    }

    using Base::react;


  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(coap_timer_ticked{}); }};
  };
}

