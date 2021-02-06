#pragma once

#include <memory>
#include <hfsm/machine_single.hpp>
#include <states/base_context.hpp>
#include <display.hpp>

struct state_machine;

namespace fsm {

  struct Context : BaseContext<state_machine> {
    Context(state_machine *sm) : BaseContext{sm} {}

    std::unique_ptr<display> disp;
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
