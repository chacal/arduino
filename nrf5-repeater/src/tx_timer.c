#include <nrf.h>
#include "tx_timer.h"

#define TX_TIMER               NRF_TIMER1
#define TX_TIMER_CC                     0
#define TX_TIMER_IRQ          TIMER1_IRQn

static tx_timer_handler_t m_timer_handler;

void tx_timer_init(tx_timer_handler_t handler) {
  m_timer_handler = handler;
  TX_TIMER->PRESCALER = 4;  // 1MHz, 1 tick == 1µs
  TX_TIMER->BITMODE   = TIMER_BITMODE_BITMODE_16Bit;  // Max timer length == ~65ms
  TX_TIMER->SHORTS    = TIMER_SHORTS_COMPARE0_STOP_Msk | TIMER_SHORTS_COMPARE0_CLEAR_Msk;  // Stop and reset timer on compare0
  TX_TIMER->INTENSET  = TIMER_INTENSET_COMPARE0_Msk;

  NVIC_SetPriority(TX_TIMER_IRQ, 4);
  NVIC_ClearPendingIRQ(TX_TIMER_IRQ);
  NVIC_EnableIRQ(TX_TIMER_IRQ);

  TX_TIMER->CC[TX_TIMER_CC] = 0;
}

bool tx_timer_stopped() {
  return TX_TIMER->CC[TX_TIMER_CC] == 0;
}

uint32_t tx_timer_time_left() {
  TX_TIMER->TASKS_CAPTURE[1] = 1;
  return TX_TIMER->CC[TX_TIMER_CC] - TX_TIMER->CC[1];
}

void tx_timer_start_with_delay(uint32_t delay) {
  tx_timer_set_delay(delay);
  TX_TIMER->TASKS_START = 1;
}

uint32_t tx_timer_get_current_delay() {
  return TX_TIMER->CC[TX_TIMER_CC];
}

void tx_timer_set_delay(uint32_t delay) {
  TX_TIMER->CC[TX_TIMER_CC] = delay;
}

void TIMER1_IRQHandler() {
  TX_TIMER->EVENTS_COMPARE[TX_TIMER_CC] = 0; // Clear compare event
  if(m_timer_handler) {
    m_timer_handler();
  }
}
