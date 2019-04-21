#pragma once

#include <hfsm/machine_single.hpp>
#include <new>
#include <rendering.hpp>

struct state_machine;

namespace fsm {

  struct Context {
    Context(state_machine *sm) : fsm{sm} {}

    template<typename T>
    void react(const T &event);

    display                 disp;
    rendering::display_list display_list;

  private:
    state_machine *fsm;
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
