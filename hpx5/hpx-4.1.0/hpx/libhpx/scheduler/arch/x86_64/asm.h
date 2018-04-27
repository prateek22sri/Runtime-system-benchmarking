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

#ifndef LIBHPX_ASM_H
#define LIBHPX_ASM_H

#include <stdint.h>

/// This file is the header that declares all of our generic assembly
/// functions. These are all implemented in an architecture-dependent way. These
/// are suitable for gcc inline assembly, but are done as asm to support
/// compilers that do not support inline asm.

extern "C" void get_mxcsr(uint32_t *out);
extern "C" void get_fpucw(uint16_t *out);
extern "C" void align_stack_trampoline(void);

#endif // LIBHPX_ASM_H
