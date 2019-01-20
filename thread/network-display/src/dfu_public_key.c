#include "sdk_config.h"
#include "stdint.h"
#include "compiler_abstraction.h"

#if NRF_CRYPTO_BACKEND_OBERON_ENABLED
/* Oberon backend is changing endianness thus public key must be kept in RAM. */
#define _PK_CONST
#else
#define _PK_CONST const
#endif


/** @brief Public key used to verify DFU images */
__ALIGN(4) _PK_CONST uint8_t pk[64] =
                                 {
                                     0xea, 0xb8, 0x9b, 0x83, 0xdc, 0x1e, 0x3e, 0x08, 0xb4, 0x3e, 0xaa, 0x9a, 0xc8, 0x41, 0x6f, 0xda, 0xac,
                                     0x72, 0x82, 0x4b, 0x75, 0xd6, 0x53, 0xf5, 0x0f, 0x62, 0xbd, 0x32, 0x2d, 0x1a, 0x49, 0x3a,
                                     0xd4, 0xc7, 0x1c, 0x05, 0xdc, 0x09, 0x8c, 0x5f, 0x8c, 0x95, 0x5c, 0x9a, 0xbe, 0x87, 0x16, 0xb1, 0xcf,
                                     0x2c, 0x37, 0xa8, 0x7a, 0xef, 0xcb, 0x33, 0x34, 0x0f, 0x47, 0xe1, 0x3e, 0x95, 0x89, 0x80
                                 };



