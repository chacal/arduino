#pragma once

#include <hfsm/machine_single.hpp>
#include <nrf_log.h>

namespace states {
  struct Context {
  };

  using M = hfsm::Machine<Context>;

  struct Idle : M::Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Idle");
    }
  };


  struct Start : M::Base {
    virtual void enter(Context &context) {
      NRF_LOG_INFO("Start");
    }

    void transition(Control &control, Context &context) {
      control.changeTo<Idle>();
    }
  };
}

class state_machine {
public:
  state_machine() : machine(ctx) {}

  bool update() {
    return machine.update();
  }

private:
  states::Context ctx;
  states::M::PeerRoot<states::Start, states::Idle> machine;
};
