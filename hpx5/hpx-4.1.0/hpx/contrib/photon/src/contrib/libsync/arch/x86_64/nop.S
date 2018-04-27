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
        
        .file "arch/x86_64/nop.S"
        /// --------------------------------------------------------------------
        /// x86_64 implementation of no ops
        ///
        /// Essentially just scavanged from gcc output for
        ///
        /// @code
        ///   __asm__ volatile ("nop");
        //    __asm__ volatile ("pause");
        /// @endcode
        /// --------------------------------------------------------------------
        INTERNAL(sync_nop)
        GLOBAL(sync_nop)
LABEL(sync_nop)
        push   %rbp
        mov    %rsp, %rbp
        nop
        pop    %rbp
        retq
        SIZE(sync_nop)

        INTERNAL(sync_pause)
        GLOBAL(sync_pause)
LABEL(sync_pause)
        push   %rbp
        mov    %rsp, %rbp
        pause
        pop    %rbp
        retq
        SIZE(sync_pause)
