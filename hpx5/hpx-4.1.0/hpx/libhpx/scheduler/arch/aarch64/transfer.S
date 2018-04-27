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

#include "reg_macros.h"
#include "../common/asm_macros.h"

        // @param x0 - a pointer to the stack pointer to transfer to
        // @param x1 - the continuation to call after the sp exchange
        // @param x2 - the worker pointer
        .file "libhpx/scheduler/arch/aarch64/transfer.S"
        .text
        .type thread_transfer, STT_FUNC
        INTERNAL(thread_transfer)
        GLOBAL(thread_transfer)
        .type thread_transfer, %function
LABEL(thread_transfer)
        STARTPROC

        sub sp, sp, 192

#define REG_PAIR(REG1, REG2, OFFS)	stp REG1, REG2, [sp, OFFS]
#define REG_ONE(REG1, OFFS)		str REG1, [sp, OFFS]
        GPR_LAYOUT
        FPR_LAYOUT
#undef REG_PAIR
#undef REG_ONE

#ifdef __VFP_FP__
        vmrs x5, fpscr
        str x5, [sp, #104]
#endif
        // We want to call cont(p, f, w, sp).
        // p is already in x0, f is already in x1, w is in x2
        // but we need to move the new sp to x3.
        mov x3, sp                              // prepare for the call
        ldr x4, [x0]                            // p->ustack      x3 = (x0)
        ldr x4, [x4]                            // p->ustack->sp  x3 = (x3)
        mov sp, x4                              // swap the stack
        bl worker_checkpoint
#ifdef __VFP_FP__
        ldr r5, [sp, #104]
#endif
	
#define REG_PAIR(REG1, REG2, OFFS)	ldp REG1, REG2, [sp, OFFS]
#define REG_ONE(REG1, OFFS)		ldr REG1, [sp, OFFS]
        GPR_LAYOUT
        FPR_LAYOUT
#undef REG_PAIR
#undef REG_ONE
        add sp, sp, 192

        ret x30
        ENDPROC
        SIZE(thread_transfer)
