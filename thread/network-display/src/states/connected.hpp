#pragma once

#include <variant>
#include <nrf_log.h>
#include <settings.hpp>
#include <coap_helpers.hpp>
#include "common.hpp"
#include "gzip.hpp"

#define COAP_TICK_PERIOD       std::chrono::seconds(1)

using namespace fsm;

namespace states {

  struct connected : Base {
    struct coap_timer_ticked {
    };

    virtual void enter(Context &context) {
      NRF_LOG_INFO("Connected");
      context.disp->on();

      auto on_display_image_post = [&](auto coap_data) { handle_display_image_post(coap_data, context); };
      coap_service::initialize({
                                 on_display_image_post,
                                 &settings::get_as_json,
                                 &settings::updateFromCoapData,
                                 &util::get_status_json
                               });
      coap_tick_timer.start(&context);
      post_state_to_mgmt_server();
    }

    virtual void react(const coap_timer_ticked &event, Control &control, Context &context) {
      coap_time_tick();
    }

    using Base::react;


  private:
    periodic_timer coap_tick_timer{COAP_TICK_PERIOD, [](void *ctx) { static_cast<Context *>(ctx)->react(coap_timer_ticked{}); }};

    void handle_display_image_post(const coap_service::post_data &coap_data, Context &context) {
      NRF_LOG_DEBUG("Got %d bytes of compressed image data", coap_data.len)
      auto result = gzip::uncompress(coap_data.data, coap_data.len);
      if (std::holds_alternative<std::vector<uint8_t>>(result)) {
        auto uncompressed = std::get<std::vector<uint8_t>>(result);
        NRF_LOG_DEBUG("Uncompressed image size: %d bytes", uncompressed.size())
        context.disp->draw_fullscreen_bitmap(uncompressed);
        context.disp->render();
      } else {
        auto error_code = std::get<int>(result);
        NRF_LOG_ERROR("Error uncompressing image! Error code: %d", error_code)
      }
    }

    void post_state_to_mgmt_server() const {
      auto state = util::create_state_post_payload(thread::get_instance());

      NRF_LOG_INFO("Posting state to mgmt server:")
      NRF_LOG_INFO("%s", state.c_str())

      coap_helpers::post_json(
        settings::m_mgmt_server, MGMT_SERVER_PORT, "v1/state/" + util::get_device_id(),
        state,
        [](uint32_t status, void *p_arg, coap_message_t *p_message) {
          NRF_LOG_INFO("State post status: %d", status)
          NRF_LOG_INFO("State post reponse code: %d", p_message->header.code)
        }, nullptr);
    }
  };
}
