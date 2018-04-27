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

#include "../common/asm_macros.h"
        
        .file "arch/arm/cptr.S"
	.syntax unified
        /// --------------------------------------------------------------------
        /// x86_64 implementation of a counted pointer
        ///
        /// This was basically scavenged from the optimized assembly of gcc.
        ///
        /// cmpxchg16b
        ///   %rdi      - *addr
        ///   %rdx:%rax - from, result
        ///   %rcx:%rbx - to
        ///
        /// callee saves
        ///    %rbp, %rbx, %r12, %r13, %r14, %r15
        /// --------------------------------------------------------------------

        INTERNAL(sync_cptr_cas_val)
        GLOBAL(sync_cptr_cas_val)
	.code 16
LABEL(sync_cptr_cas_val)
	push {r5, lr}
	blx sync_cptr_cas_val_sub
	pop {r5, lr}
	blx lr
	
	.code 32
	LABEL(sync_cptr_cas_val_sub)
        push    {r5-r8}
        mov     r5, r0                  // r5 = ptr
        mov     r6, r1                  // r6 = from
        mov     r7, r2                  // r7 = to
        ldrd    r2, r3, [r1]
        ldrexd  r0, r1, [r5]            // r0, r1 = *ptr
                                        // need to read two words, so we need
                                        // to use an exclusive read
        cmp     r2, r0
        cmpeq   r3, r1
        bl      sync_cptr_cas_val.fail
LABEL(sync_cptr_cas_val.sucd)
        add     r1, #1                  // ptr.c++
        mov     r0, r7                  // ptr.p = to
        strexd  r8, r0, r1, [r5]        // *ptr = {to, ptr.c}
        cmp     r8, #0                  // if (success)
	it      eq
        bleq    sync_cptr_cas_val.epi   //   then go to epilog
LABEL(sync_cptr_cas_val.fail)
        clrex
        strd    r0, r1, [r5]            // *from = *ptr
LABEL(sync_cptr_cas_val.epi)
        pop     {r5-r8}
        bx      lr
        SIZE(sync_cptr_cas_val)

        INTERNAL(sync_cptr_cas)
        GLOBAL(sync_cptr_cas)
	.code 16
LABEL(sync_cptr_cas)
	push {r5, lr}
	blx sync_cptr_cas_sub
	pop {r5, lr}
	blx lr
	
	.code 32
LABEL(sync_cptr_cas_sub)
	push    {r5-r8}
        mov     r5, r0                  // r5 = ptr
        mov     r6, r1                  // r6 = from
        mov     r7, r2                  // r7 = to
        ldrd    r2, r3, [r1]
        ldrexd  r0, r1, [r5]            // r0, r1 = *ptr
                                        // need to read two words, so we need
                                        // to use an exclusive read
        cmp     r2, r0
	it      eq
        cmpeq   r3, r1
        bl      sync_cptr_cas.fail
LABEL(sync_cptr_cas.sucd)
        add     r1, #1                  // ptr.c++
        mov     r0, r7                  // ptr.p = to
        strexd  r8, r0, r1, [r5]        // try *ptr = {to, ptr.c}
                                        // set r0 to success
        eor     r0, r8, #1              // convert status to C bool and put in return
        bl      sync_cptr_cas.epi

LABEL(sync_cptr_cas.fail)
        clrex
        mov     r0, #0

LABEL(sync_cptr_cas.epi)
        pop     {r5-r7}
        bx      lr
        SIZE(sync_cptr_cas)

        INTERNAL(sync_cptr_load)
        GLOBAL(sync_cptr_load)
	.code 16
LABEL(sync_cptr_load)
	push {r5, lr}
	blx sync_cptr_load_sub
	pop {r5, lr}
	blx lr
	
	.code 32
LABEL(sync_cptr_load_sub)

        ldrexd  r2, r3, [r0]            // need to read two words, so we need
                                        // to use an exclusive read
        strd    r2, r3, [r1]            // should this be a strexd??? I don't
                                        // think so but I could be wrong
        clrex                           // have to clear the exclusive monitor
                                        // if we haven't done an exclusive store
        bx      lr
        SIZE(sync_cptr_load)
        
        INTERNAL(sync_cptr_is_consistent)
            GLOBAL(sync_cptr_is_consistent)
	.code 16
LABEL(sync_cptr_is_consistent)
	push {r5, lr}
	blx sync_cptr_cas_sub
	pop {r5, lr}
	blx lr
	
	.code 32
LABEL(sync_cptr_is_consistent_sub)
        ldrd    r2, r3, [r1]
        ldrexd  r0, r1, [r0]            // need to read two words, so we need
                                        // to use an exclusive read
        clrex                           // have to clear the exclusive monitor
                                        // if we haven't done an exclusive store
        cmp     r2, r0
	itte    eq                      // if r2 == r0 then (cmp r3, r1; r0 = 1) else (r0 = 0)
        cmpeq   r3, r1                  //   if r2 == r0 then cmp r3, r1
        moveq   r0, #1                  //   if r2 == r0 and r3 == r1 then r0 = 1
        movne   r0, #0                  //   else then r0 = 0
        
        bx      lr
        SIZE(sync_cptr_is_consistent)
