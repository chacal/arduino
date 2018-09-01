#include <nrf_drv_rng.h>
#include <stdlib.h>

#include "random.h"

static nrf_drv_rng_config_t m_rng_config = NRF_DRV_RNG_DEFAULT_CONFIG;

void random_init() {
  nrf_drv_rng_init(&m_rng_config);
  uint8_t seed[sizeof(unsigned int)];
  nrf_drv_rng_block_rand(seed, sizeof(seed));
  srand(*(unsigned int *) seed);
  nrf_drv_rng_uninit();
}

uint32_t random_get() {
  return (uint32_t)rand();
}
