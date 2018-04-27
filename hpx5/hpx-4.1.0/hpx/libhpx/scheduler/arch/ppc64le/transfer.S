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
#include "ppc-asm.h"

        // @param r3 - a pointer to the stack pointer to transfer to
        // @param r4 - the continuation to call after the sp exchange
        // @param r5 - the worker
        .file "libhpx/scheduler/arch/ppc64le/transfer.S"
        .machine power8
        .abiversion 2
        .section        .text
        .align 2
        .globl thread_transfer
        .type thread_transfer, @function
LABEL(thread_transfer)
        STARTPROC
.LCF1:
        // Global entry of thread_transfer
        // TOC pointer initialization code sequence
        // r12 holds the current function entry address
        // .TOC - TOC Pointer
        // Initialize the value of the TOC pointer into r2
        // Calculate the upper 16 bits
0:      addis 2, 12, .TOC.-.LCF1@ha
        // Add the low 16 bits
	addi 2, 2, .TOC.-.LCF1@l
       
        // Local entry of thread_transfer
        .localentry thread_transfer,.-thread_transfer

        # PROLOGUE
        # 1) Reserve stack space and save the old stack pointer
        # This will allocate stack memory atomically.
        stdu 1,-336(1)
        # Save LR
        mflr 0
        std  0, 0*8(1)

        # Save CR
        mfcr 8
        std  8, 1*8(1)

        # store r2 (TOC)
        std   2, 2*8(1)

        # Save all non-volatile registers that will be used
        # during this function
        std  14, 4*8(1)
        std  15, 5*8(1)
        std  16, 6*8(1)
        std  17, 7*8(1)
        std  18, 8*8(1)
        std  19, 9*8(1)
        std  20, 10*8(1)
        std  21, 11*8(1)
        std  22, 12*8(1)
        std  23, 13*8(1)
        std  24, 14*8(1)
        std  25, 15*8(1)
        std  26, 16*8(1)
        std  27, 17*8(1)
        std  28, 18*8(1)
        std  29, 19*8(1)
        std  30, 20*8(1)
        std  31, 21*8(1)

        stfd 14, 22*8(1)
        stfd 15, 23*8(1)
        stfd 16, 24*8(1)
        stfd 17, 25*8(1)
        stfd 18, 26*8(1)
        stfd 19, 27*8(1)
        stfd 20, 28*8(1)
        stfd 21, 29*8(1)
        stfd 22, 30*8(1)
        stfd 23, 31*8(1)
        stfd 24, 32*8(1)
        stfd 25, 33*8(1)
        stfd 26, 34*8(1)
        stfd 27, 35*8(1)
        stfd 28, 36*8(1)
        stfd 29, 37*8(1)
        stfd 30, 38*8(1)
        stfd 31, 39*8(1)

        // r0 commonly used to hold the old link register when building the stack frame
        // r1 is the stack pointer 
        // r2 dedicated table of contents pointer
        // r3 Volatile parameter and return value register
        // r4-r10 Volatile registers used for function parameters
        // We want to call cont(p, f, w, sp).
        // p is there in r3, f in r4 and this is in r5
        // but we need to move the new sp to x6.
        mr 6, 1     // Checkpoint sp for call
        ld 7, 0(3)  // p->ustack r7 = (r3)
        ld 7, 0(7)  // p->ustack->sp r7 = (r7)
        mr 1, 7     // Replace r1 (sp) for the context switch

        stwu 1, -48(1)
        bl worker_checkpoint
        nop
        addi 1, 1, 48
        # move old return address into the link register
        ld   0, 0*8(1)
        mtlr 0
        # move cr
        ld  8, 1*8(1)
        # Restore CR2, CR3, CR4 of the CR
        mtcrf 32, 8
        mtcrf 16, 8
        mtcrf  8, 8

        # restore r2 (TOC)
        ld   2, 2*8(1)
        ld   3, 3*8(1)
        # The function's epilogue follows the reverse sequence, restoring what had been saved
        ld  14, 4*8(1)
        ld  15, 5*8(1)
        ld  16, 6*8(1)
        ld  17, 7*8(1)
        ld  18, 8*8(1)
        ld  19, 9*8(1)
        ld  20, 10*8(1)
        ld  21, 11*8(1)
        ld  22, 12*8(1)
        ld  23, 13*8(1)
        ld  24, 14*8(1)
        ld  25, 15*8(1)
        ld  26, 16*8(1)
        ld  27, 17*8(1)
        ld  28, 18*8(1)
        ld  29, 19*8(1)
        ld  30, 20*8(1)
        ld  31, 21*8(1)

        lfd 14, 22*8(1)
        lfd 15, 23*8(1)
        lfd 16, 24*8(1)
        lfd 17, 25*8(1)
        lfd 18, 26*8(1)
        lfd 19, 27*8(1)
        lfd 20, 28*8(1)
        lfd 21, 29*8(1)
        lfd 22, 30*8(1)
        lfd 23, 31*8(1)
        lfd 24, 32*8(1)
        lfd 25, 33*8(1)
        lfd 26, 34*8(1)
        lfd 27, 35*8(1)
        lfd 28, 36*8(1)
        lfd 29, 37*8(1)
        lfd 30, 38*8(1)
        lfd 31, 39*8(1)

        # Restore stack frame
        addi 1, 1, 336

        # Return
        blr
        ENDPROC
        SIZE(thread_transfer)
