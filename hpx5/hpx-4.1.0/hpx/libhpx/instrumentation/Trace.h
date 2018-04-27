// ==================================================================-*- C++ -*-
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef LIBHPX_INSTRUMENTATION_TRACE_H
#define LIBHPX_INSTRUMENTATION_TRACE_H

#include "libhpx/config.h"
#include <cstdarg>
#include <atomic>

namespace libhpx {
namespace instrumentation {
class Trace {
 public:
  virtual ~Trace();

  std::atomic<bool> active;

  virtual void start(void) = 0;
  virtual void destroy(void) = 0;
  virtual void vappend(int type, int n, int id, va_list&) = 0;

 protected:
  Trace(const config_t *cfg);
};
} // namespace instrumentation
} // namespace libhpx

#endif // LIBHPX_INSTRUMENTATION_TRACE_H
