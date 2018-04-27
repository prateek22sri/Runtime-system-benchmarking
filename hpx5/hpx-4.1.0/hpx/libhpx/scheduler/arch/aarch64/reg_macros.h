// =============================================================================
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

#ifndef LIBHPX_REG_MACROS_H
#define LIBHPX_REG_MACROS_H

#define GPR_LAYOUT			\
	REG_PAIR (x19, x20,  0);	\
	REG_PAIR (x21, x22, 16);	\
	REG_PAIR (x23, x24, 32);	\
	REG_PAIR (x25, x26, 48);	\
	REG_PAIR (x27, x28, 64);	\
	REG_PAIR (x29, x30, 80);

#define FPR_LAYOUT			\
	REG_PAIR ( d8,  d9, 112);	\
	REG_PAIR (d10, d11, 128);	\
	REG_PAIR (d12, d13, 144);	\
	REG_PAIR (d14, d15, 160);

#endif // LIBHPX_REG_MACROS_H
