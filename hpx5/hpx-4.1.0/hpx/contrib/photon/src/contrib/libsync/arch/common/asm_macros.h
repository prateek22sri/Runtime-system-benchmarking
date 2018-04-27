// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2015, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef LIBHPX_ASM_MACROS_H
#define LIBHPX_ASM_MACROS_H

#if defined(__APPLE__)
#define GLOBAL(S) .globl _##S
#define LABEL(S) _##S:
#define INTERNAL(S) .private_extern _##S
#define SIZE(S)
#elif defined(__linux__)
#define GLOBAL(S) .globl S
#define LABEL(S) S:
#define INTERNAL(S) .internal S
#define SIZE(S) .size S, .-S
#else
#error No ASM support for your platform.
#endif

#endif // LIBHPX_ASM_MACROS_H
