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

#include "../common/asm_macros.h"

        /// Implementation of some simple asm utilities.
        ///
        /// These could be done simply as inline asm if we only wanted to
        /// support gcc-compatible inline asm implementations, but we care about
        /// CrayCC and we haven't seen such support there yet. This costs us a
        /// function call (and associated caller-side register churn) for these
        /// operations.
        .file "libhpx/scheduler/arch/ppc64le/asm.S"
        .machine power8
        .abiversion 2
        .section        .text
        .align 2
        .globl align_stack_trampoline
        .type align_stack_trampoline, @function
LABEL(align_stack_trampoline)
        STARTPROC
	mtlr 0x0
        mr 3, 15
        mr 12, 14
        mtctr 12
        bctrl
        ENDPROC
        SIZE(align_stack_trampoline)
