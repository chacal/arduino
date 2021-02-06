template<typename SM>
struct BaseContext {
  BaseContext(SM *sm) : fsm{sm} {}

  template<typename T>
  void react(const T &event) {
    fsm->react(event);
  }

  std::string mgmt_server;

protected:
  SM *fsm;
};
