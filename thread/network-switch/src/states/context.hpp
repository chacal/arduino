#pragma once

#include <memory>
#include <hfsm/machine_single.hpp>
#include <states/base_context.hpp>

struct state_machine;

namespace fsm {

  struct Context : BaseContext<state_machine> {
    Context(state_machine *sm) : BaseContext{sm} {}
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
