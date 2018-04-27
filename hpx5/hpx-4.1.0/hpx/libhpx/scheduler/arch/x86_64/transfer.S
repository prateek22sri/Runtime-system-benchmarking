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
        
        /// The x86_64 transfer implementation.
        ///
        /// Checkpoints the current callee-saves state on the current thread's
        /// stack, then exchanges stack pointers to start (or resume) execution
        /// on the target stack, then calls the continuation with the old stack
        /// pointer, and restores the checkpoint from the target thread's
        /// stack.
        ///
        /// For newly allocated stacks, we have to "forge" a thread_transfer()
        /// frame that we can start up through a thread_transfer() to it. We
        /// know what the frame looks like, so we can manufacture one. The only
        /// sneakiness in doing this is that we need to make sure that the stack
        /// pointer is aligned correctly, which is what the
        /// align_rsp_trampoline is for. See frame.c for details.
        ///
        /// @param %rdi A pointer to the stack pointer to transfer to.
        /// @param %rsi The continuation to call after the sp exchange.
        /// @param %rdx The environment for the continutation.
        ///
        /// @returns %rax from the continuation call

        .file "transfer.S"
        .text
        SECTION(rodata)
        TYPE(thread_transfer, @function)
        INTERNAL(thread_transfer)
        GLOBAL(thread_transfer)
LABEL(thread_transfer)
        STARTPROC
        push %rbp
        CFA_ADJ_OFFSET(8)
	CFI_REL_OFFSET(%rbp, 0)
        push %rbx
        CFA_ADJ_OFFSET(8)
	CFI_REL_OFFSET(%rbx, 0)
        push %r12
        CFA_ADJ_OFFSET(8)
	CFI_REL_OFFSET(%r12, 0)
        push %r13
        CFA_ADJ_OFFSET(8)
	CFI_REL_OFFSET(%r13, 0)
        push %r14
        CFA_ADJ_OFFSET(8)
	CFI_REL_OFFSET(%r14,0)
        push %r15
        CFA_ADJ_OFFSET(8)
	CFI_REL_OFFSET(%r15,0)
        sub $8, %rsp
        CFA_ADJ_OFFSET(8)
        fnstcw 4(%rsp)
        stmxcsr	(%rsp)
        mov (%rdi), %rcx                        // p->stack
        mov (%rcx), %rcx                        // p->stack->sp
        xchg %rsp, %rcx
	CFA_REGISTER(rcx)
        call PLT_SYMBOL(worker_checkpoint)
	CFA_DEF(rsp,64)
        ldmxcsr	(%rsp)
        fldcw 4(%rsp)
        add $8, %rsp
        CFA_ADJ_OFFSET(-8)
        pop %r15
	CFI_RESTORE(r15)
        CFA_ADJ_OFFSET(-8)
        pop %r14
	CFI_RESTORE(r14)
        CFA_ADJ_OFFSET(-8)
        pop %r13
	CFI_RESTORE(r13)
        CFA_ADJ_OFFSET(-8)
        pop %r12
	CFI_RESTORE(r12)
        CFA_ADJ_OFFSET(-8)
        pop %rbx
	CFI_RESTORE(rbx)
        CFA_ADJ_OFFSET(-8)
        pop %rbp
	CFI_RESTORE(rbp)
        CFA_ADJ_OFFSET(-8)
        ret
        ENDPROC
        SIZE(thread_transfer)


