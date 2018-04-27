// ================================================================= -*- C++ -*-
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

#ifndef HPX_CXX_ERRORS_H
#define HPX_CXX_ERRORS_H

#include <exception>
#include <hpx/types.h>

namespace hpx {
/// For now C++ API throws errors that correspond to the HPX hpx_status_t.
struct Error : public std::exception {
  Error() : std::exception(), code(HPX_ERROR) {
  }

  Error(hpx_status_t c) : std::exception(), code(c) {
  }

  virtual const char* what() const throw() {
    return hpx_strerror(code);
  }

  hpx_status_t code;
};
}

#endif // HPX_CXX_ERRORS_H
