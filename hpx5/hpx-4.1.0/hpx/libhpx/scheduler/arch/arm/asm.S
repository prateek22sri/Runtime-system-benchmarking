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
        .file "libhpx/scheduler/arch/arm/asm.S"
        .text
        .syntax unified
        .code 32
        .type align_stack_trampoline, STT_FUNC
        INTERNAL(align_stack_trampoline)
        GLOBAL(align_stack_trampoline)
LABEL(align_stack_trampoline)
        STARTPROC
        CFA_DEF_OFFSET(8)
        sub sp, sp, #8
        eor r0, r0, r0
        str r0, [sp]
        str r0, [sp, #4]
        mov r0, r5
        bx  r4                  // which register is arbitrary, but it is
                                // controlled in frame.c
        ENDPROC
        SIZE(align_stack_trampoline)

