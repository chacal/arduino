#pragma once

#include <memory>
#include <hfsm/machine_single.hpp>
#include <display.hpp>

struct state_machine;

namespace fsm {

  struct Context {
    Context(state_machine *sm) : fsm{sm} {}

    template<typename T>
    void react(const T &event);

    std::unique_ptr<display> disp;
  private:
    state_machine *fsm;
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
