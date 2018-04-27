// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

/* Use, modification and distribution is subject to the Boost Software     */
/* License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at */
/* http://www.boost.org/LICENSE_1_0.txt)                                   */
/*                                                                         */
/*  Authors: Jeremiah Willcock                                             */
/*           Andrew Lumsdaine                                              */

#ifndef MOD_ARITH_H
#define MOD_ARITH_H

//#include "user_settings.h"

/* Various modular arithmetic operations for modulus 2^31-1 (0x7FFFFFFF).
 * These may need to be tweaked to get acceptable performance on some platforms
 * (especially ones without conditional moves). */

/* This code is now just a dispatcher that chooses the right header file to use
 * per-platform. */

#ifdef __MTA__
#include "mod_arith_xmt.h"
#else
#ifdef FAST_64BIT_ARITHMETIC
#include "mod_arith_64bit.h"
#else
#include "mod_arith_32bit.h"
#endif
#endif

#endif /* MOD_ARITH_H */
