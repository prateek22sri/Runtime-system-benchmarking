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

#ifndef LIBHPX_ASM_MACROS_H
#define LIBHPX_ASM_MACROS_H

#if defined(HAVE_CFI_DIRECTIVES)
# define STARTPROC .cfi_startproc
# define ENDPROC .cfi_endproc
# define CFA_DEF_OFFSET(N) .cfi_def_cfa_offset N
# define CFA_ADJ_OFFSET(N) .cfi_adjust_cfa_offset N
# define CFA_REGISTER(R) .cfi_def_cfa_register R
# define CFA_DEF(R,N) .cfi_def_cfa R,N
# define CFI_RESTORE(R) .cfi_restore R
# define CFI_OFFSET(R,N) .cfi_offset R,N
# define CFI_REL_OFFSET(R,N) .cfi_rel_offset R,N
#else
# define STARTPROC
# define ENDPROC
# define CFA_DEF_OFFSET(N)
# define CFA_ADJ_OFFSET(N)
# define CFA_REGISTER(R)
# define CFA_DEF(R,N)
# define CFI_RESTORE(R)
# define CFI_OFFSET(R,N)
# define CFI_REL_OFFSET(R,N)
#endif


#if defined(__APPLE__)
# define SYMBOL(S) _##S
# define GLOBAL(S) .globl SYMBOL(S)
# define PLT_SYMBOL(S) _##S
# define LABEL(S) SYMBOL(S):
# define INTERNAL(S) .private_extern SYMBOL(S)
# define SIZE(S)
# define TYPE(S, T)
# define SECTION(S)
#elif defined(__linux__)
# define SYMBOL(S) S
# define PLT_SYMBOL(S) S##@PLT
#if defined(__x86_64__) || defined(__ARMEL__) || defined(__PPC64__)
# define GLOBAL(S) .globl SYMBOL(S)
#elif defined(__aarch64__)
# define GLOBAL(S) .global SYMBOL(S)
#else
#error GLOBAL(S) not defined for your processor
#endif
# define LABEL(S) SYMBOL(S):
# define INTERNAL(S) .internal SYMBOL(S)
# define SIZE(S) .size SYMBOL(S), .-SYMBOL(S)
# define TYPE(S, T) .type SYMBOL(S), T
# define SECTION(S) .section .S
#else
#error No ASM support for your platform.
#endif

#endif // LIBHPX_ASM_MACROS_H
