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

/// @file include/hpx/hpx++.h
/// @brief The Main HPX++ header file.
///
/// To use the HPX++ API only this header is needed; all the other HPX++
/// headers are included through it.

#ifndef HPX_PLUS_PLUS_H
#define HPX_PLUS_PLUS_H

#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <hpx/hpx.h>
#include <hpx/cxx/action.h>
#include <hpx/cxx/ccall.h>
#include <hpx/cxx/errors.h>
#include <hpx/cxx/gas.h>
#include <hpx/cxx/global_ptr.h>
#include <hpx/cxx/lco.h>
#include <hpx/cxx/malloc.h>
#include <hpx/cxx/par_for.h>
#include <hpx/cxx/process.h>
#include <hpx/cxx/runtime.h>
#include <hpx/cxx/string.h>
#include <hpx/cxx/system.h>
#include <hpx/cxx/thread.h>

namespace hpx {
/// Export the error codes into the HPX namespace as integers. We could use an
/// enum or something but this keeps the typing consistent.
/// @{
const int SUCCESS = HPX_SUCCESS;
const int ERROR = HPX_ERROR;
/// @}
}

#endif // HPX_PLUS_PLUS_H
