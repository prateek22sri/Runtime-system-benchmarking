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
#include "asm_macros.h"
        
        .file "arch/x86_64/cptr.S"
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
LABEL(sync_cptr_cas_val)
        push   %rbx
        mov    %rdx, %rbx                       // %rbx = to
        mov    0x8(%rsi), %rdx                  // %rdx = from.count
        mov    (%rsi), %rax                     // %rax = from.p
        lea    0x1(%rdx), %rcx                  // %rcx = from.count + 1
        lock
        cmpxchg16b (%rdi)
        pop    %rbx
        ret
	    SIZE(sync_cptr_cas_val)

	    INTERNAL(sync_cptr_cas)
	    GLOBAL(sync_cptr_cas)
LABEL(sync_cptr_cas)
        push   %rbx
        mov    %rdx, %rbx                       // %rbx = to
        mov    0x8(%rsi), %rdx                  // %rdx = from.count
        mov    (%rsi), %rax                     // %rax = from.p
        lea    0x1(%rdx), %rcx                  // %rcx = from.count + 1
        lock
        cmpxchg16b (%rdi)
        sete   %al                              // 
        pop    %rbx
        ret
	    SIZE(sync_cptr_cas)

        INTERNAL(sync_cptr_load)
	    GLOBAL(sync_cptr_load)
LABEL(sync_cptr_load)
        push   %rbx
        mov    %rax, %rbx
        mov    %rdx, %rcx
        lock        
        cmpxchg16b (%rdi)
        mov    %rdx, 0x8(%rsi)
        mov    %rax, (%rsi)
        pop    %rbx
        retq
        SIZE(sync_cptr_load)

        INTERNAL(sync_cptr_is_consistent)
	    GLOBAL(sync_cptr_is_consistent)
LABEL(sync_cptr_is_consistent)
        push   %rbx
        mov    0x8(%rsi), %rdx                  // %rdx = from.count
        mov    (%rsi), %rax                     // %rax = from.p
        mov    %rdx, %rcx                       // %rcx = from.count
        mov    %rax, %rbx                       // %rbx = from.p
        lock
        cmpxchg16b (%rdi)
        sete   %al                              // 
        pop    %rbx
        ret
        SIZE(sync_cptr_is_consistent)
