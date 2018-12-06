#include <openthread/thread.h>
#include "nrf_log_ctrl.h"
#include "nrf_log.h"
#include "nrf_log_default_backends.h"

#include "thread_utils.h"

static void thread_state_changed_callback(uint32_t flags, void *p_context) {
  NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d",
               flags, otThreadGetDeviceRole(p_context));
}

static void thread_instance_init(void) {
  thread_configuration_t thread_configuration =
                             {
                                 .role                  = RX_ON_WHEN_IDLE,
                                 .autocommissioning     = true,
                                 .autostart_disable     = false,
                             };

  thread_init(&thread_configuration);
  thread_cli_init();
  thread_state_changed_callback_set(thread_state_changed_callback);
}



int main(int argc, char *argv[]) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("Starting Thread FTD..")

  while (true) {
    thread_instance_init();

    while (!thread_soft_reset_was_requested()) {
      thread_process();

      if (NRF_LOG_PROCESS() == false) {
        thread_sleep();
      }
    }

    thread_soft_deinit();
  }
}
