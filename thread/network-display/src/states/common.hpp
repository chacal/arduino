#pragma once

#include <hfsm/machine_single.hpp>
#include <rendering.hpp>
#include <eink_display/eink_display.hpp>

struct state_machine;

namespace fsm {

  struct Context {
    Context(state_machine *sm) : fsm{sm} {}

    template<typename T>
    void react(const T &event);

    std::unique_ptr<display> disp = std::make_unique<eink_display>();
    rendering::display_list  display_list;

  private:
    state_machine *fsm;
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
