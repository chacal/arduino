
namespace states {
  template<typename M>
  struct base_configure : M::Base {
  protected:
    struct config_timer_elapsed {
    };
    struct coap_timer_ticked {
    };

  public:
    virtual void enter(typename M::Context &context) {
      NRF_LOG_INFO("Configuring..");
      config_timer.start(&context);
      coap_tick_timer.start(&context);
      get_configuration(context);
    }

    virtual void leave(typename M::Context &context) {
      config_timer.stop();
      coap_tick_timer.stop();
    }

    virtual void react(const coap_timer_ticked &event, typename M::Control &control, typename M::Context &context) {
      coap_time_tick();
    }

    virtual void react(const config_timer_elapsed &event, typename M::Control &control, typename M::Context &context) {
      get_configuration(context);
    }

  protected:
    virtual void get_configuration(typename M::Context &context) = 0;


  private:
    milliseconds   coap_tick_period    = std::chrono::seconds(1);
    milliseconds   config_timer_period = std::chrono::seconds(60);
    periodic_timer coap_tick_timer{coap_tick_period,
                                   [](void *ctx) { static_cast<typename M::Context *>(ctx)->react(coap_timer_ticked{}); }};
    periodic_timer config_timer{config_timer_period,
                                [](void *ctx) { static_cast<typename M::Context *>(ctx)->react(config_timer_elapsed{}); }};
  };
}