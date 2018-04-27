// =============================================================================
//  Photon RDMA Library (libphoton)
//
//  Copyright (c) 2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#include <stdarg.h>
#include "logging.h"

void photon_logging_msg(FILE *f, const char *p, unsigned line, const char *func,
			const char *fmt, ...) {
  fprintf(f, "%s: %d (%s:%d): > ", p, _photon_myrank, func, line);
  va_list args;
  va_start(args, fmt);
  vfprintf(f, fmt, args);
  va_end(args);
  fprintf(f, "\n");
  fflush(f);
}
