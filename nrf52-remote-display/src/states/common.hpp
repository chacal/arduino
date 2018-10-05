#pragma once

#include <hfsm/machine_single.hpp>
#include "display.hpp"

struct state_machine;

namespace fsm {

  struct Context {
    Context(state_machine *sm, display &d) : disp(d) , fsm{sm} {}

    template<typename T>
    void react(const T &event);
    display &disp;
    uint16_t conn_handle;

  private:
    state_machine *fsm;
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
