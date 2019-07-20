#include "gzip.hpp"

#include <nrf_log.h>
#include "uzlib.h"

#define OUT_CHUNK_SIZE 16

static bool uzlib_initialized = false;

namespace gzip {

  result uncompress(const uint8_t *source, uint16_t len) {
    if (!uzlib_initialized) {
      uzlib_init();
      uzlib_initialized = true;
    }

    uint16_t dlen;

    dlen = source[len - 1];
    dlen = 256 * dlen + source[len - 2];
    dlen = 256 * dlen + source[len - 3];
    dlen = 256 * dlen + source[len - 4];

    /* there can be mismatch between length in the trailer and actual
     data stream; to avoid buffer overruns on overlong streams, reserve
     one extra byte */
    dlen++;

    std::vector<uint8_t> dest(dlen);

    struct uzlib_uncomp d = {0};
    uzlib_uncompress_init(&d, nullptr, 0);

    d.source         = source;
    d.source_limit   = source + len - 4;
    d.source_read_cb = nullptr;

    int res = uzlib_gzip_parse_header(&d);
    if (res != TINF_OK) {
      return res;
    }

    d.dest_start = d.dest = dest.data();

    while (dlen) {
      unsigned int chunk_len = dlen < OUT_CHUNK_SIZE ? dlen : OUT_CHUNK_SIZE;
      d.dest_limit = d.dest + chunk_len;
      res = uzlib_uncompress_chksum(&d);
      dlen -= chunk_len;
      if (res != TINF_OK) {
        break;
      }
    }

    if (res != TINF_DONE) {
      return res;
    }

    NRF_LOG_DEBUG("Decompressed %d bytes", d.dest - dest.data())

    dest.pop_back();  // Remove the extra allocated buffer byte

    return dest;
  }
}