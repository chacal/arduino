#pragma once

#include <hfsm/machine_single.hpp>

struct state_machine;

namespace states {

  struct Context {
    Context(state_machine *sm) : fsm{sm} {}

    template<typename T>
    void react(const T &event);

  private:
    state_machine *fsm;
  };

  using M = hfsm::Machine<Context>;
}
