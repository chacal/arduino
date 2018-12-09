#include <openthread/thread.h>
#include <thread_utils.h>
#include <nrf_log_ctrl.h>
#include <nrf_log.h>
#include <nrf_log_default_backends.h>

#include "thread.h"


int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("Starting Thread Router..")

  while (true) {
    thread_initialize();

    while (!thread_soft_reset_was_requested()) {
      thread_process();
      thread_sleep();
    }

    thread_soft_deinit();
  }
}
