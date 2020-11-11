#pragma once

#include <memory>
#include <hfsm/machine_single.hpp>
#include <display.hpp>
#include <eink_display/good_display/good_display_2in9_4gray.hpp>

struct state_machine;

namespace fsm {

  struct Context {
    Context(state_machine *sm) : fsm{sm} {}

    template<typename T>
    void react(const T &event);

    std::unique_ptr<display> disp = std::make_unique<good_display_2in9_4gray>();
    std::string              mgmt_server_address;
    std::string              instance;
  private:
    state_machine *fsm;
  };

  using M = hfsm::Machine<Context>;
  using Base = M::Base;
}
