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

        // @param r0 - a pointer to the stack pointer to transfer to
        // @param r1 - the continuation to call after the sp exchange
        // @param r2 - the worker
        .file "libhpx/scheduler/arch/arm/transfer.S"
        .text
        .syntax unified
        .code 32
        .type thread_transfer, STT_FUNC
        INTERNAL(thread_transfer)
        GLOBAL(thread_transfer)
LABEL(thread_transfer)
        STARTPROC
        push {lr}
        CFA_ADJ_OFFSET(4)
        CFI_REL_OFFSET(lr, 0)
        push {r4}
        CFA_ADJ_OFFSET(4)
        CFI_REL_OFFSET(r4, 0)
        push {r5}
        CFA_ADJ_OFFSET(4)
        CFI_REL_OFFSET(r5, 0)
        stmdb sp!, {r6-r11} // may not need r9?
        CFA_ADJ_OFFSET(24)
        CFI_REL_OFFSET(r6, 0)
        CFI_REL_OFFSET(r7, 4)
        CFI_REL_OFFSET(r8, 8)
        CFI_REL_OFFSET(r9, 12)
        CFI_REL_OFFSET(r10, 16)
        CFI_REL_OFFSET(r11, 20)

#if __VFP_FP__
        // if VFP and/or Advanced SIMD (i.e. NEON) is enabled, we need
        // to back up the floating point registers and the control
        // register.
        // We can check for NEON support directly with __ARM_NEON__ but
        // I believe __VFP_FP__ is set whenever __ARM_NEON__ is set. The
        // registers are the same.
        vstmdb sp!, {s16-s31}
        CFA_ADJ_OFFSET(64)
        sub sp, sp, #8 // could be 4 but we want to keep stack 8 byte aligned
        CFA_ADJ_OFFSET(8)
        vmrs r5, fpscr
        str r5, [sp, #8]
#endif
        sub sp, sp, #4 // keep stack alignment at 8 bytes
        CFA_ADJ_OFFSET(4)

        // We want to call cont(p, sp, env).
        // p is already in r0 and env is already in r2,
        // but we need to move the new sp to r1.
        mov r3, sp                              // checkpoint sp for call
        ldr r4, [r0]                            // p->ustack      r4 = (r0)
        ldr r4, [r4]                            // p->ustack->sp  r4 = (r4)
        mov sp, r4                              // replace sp for the context switch
        CFA_REGISTER(r1)
        bl worker_checkpoint
        CFA_DEF(sp, 56)

        add sp, sp, #4
        CFA_ADJ_OFFSET(-4)
#ifdef __VFP_FP__
        ldr r5, [sp, #8]
        add sp, sp, #8
        CFA_ADJ_OFFSET(-8)
        vldmia sp!, {s16-s31}
        CFA_ADJ_OFFSET(-64)
#endif
        ldmia sp!, {r6-r11}
        CFA_ADJ_OFFSET(-24)
        CFI_RESTORE(r6)
        CFI_RESTORE(r7)
        CFI_RESTORE(r8)
        CFI_RESTORE(r9)
        CFI_RESTORE(r10)
        CFI_RESTORE(r11)
        pop {r5}
        CFA_ADJ_OFFSET(-4)
        CFI_RESTORE(r5)
        pop {r4}
        CFA_ADJ_OFFSET(-4)
        CFI_RESTORE(r4)
        pop {lr}
        CFA_ADJ_OFFSET(-4)
        CFI_RESTORE(lr)
        bx lr
        ENDPROC
        SIZE(thread_transfer)
