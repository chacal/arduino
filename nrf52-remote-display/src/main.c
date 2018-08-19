#include <nrf_log_default_backends.h>
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


static void power_manage(void) {
  // Always clear FPU IRQs to allow CPU to sleep. See: https://devzone.nordicsemi.com/question/87838/high-power-consumption-when-using-fpu/
  __set_FPSCR(__get_FPSCR() & ~(0x0000009F));
  (void) __get_FPSCR();
  NVIC_ClearPendingIRQ(FPU_IRQn);
  __WFE();
  __WFI();
}


int main(void) {
  APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
  NRF_LOG_DEFAULT_BACKENDS_INIT();

  NRF_LOG_INFO("Hello world!")

  for(;;) {
    power_manage();
  }
}
