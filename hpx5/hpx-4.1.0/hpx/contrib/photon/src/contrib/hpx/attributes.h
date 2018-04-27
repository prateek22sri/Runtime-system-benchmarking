// =============================================================================
//  High Performance ParalleX Library (libhpx)
//
//  Copyright (c) 2013-2016, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// =============================================================================

#ifndef HPX_ATTRIBUTES_H
#define HPX_ATTRIBUTES_H

#ifdef __cplusplus
extern "C" {
#endif

/// @file
/// @brief Attribute definitions for HPX functions.

#define HPX_ATTRIBUTE(S)     __attribute__(S)
#define HPX_ALIGNED(...)     HPX_ATTRIBUTE((aligned(__VA_ARGS__)))
#define HPX_RETURNS_TWICE    HPX_ATTRIBUTE((returns_twice))
#define HPX_NORETURN         HPX_ATTRIBUTE((noreturn))
#define HPX_NOINLINE         HPX_ATTRIBUTE((noinline))
#define HPX_ALWAYS_INLINE    HPX_ATTRIBUTE((always_inline))
#define HPX_OPTIMIZE(S)      HPX_ATTRIBUTE((optimize(S)))
#define HPX_MALLOC           HPX_ATTRIBUTE((malloc))
#define HPX_USED             HPX_ATTRIBUTE((used))
#define HPX_UNUSED           HPX_ATTRIBUTE((unused))
#define HPX_ASM(S)           __asm__(#S)
#define HPX_PACKED           HPX_ATTRIBUTE((packed))
#define HPX_NON_NULL(...)    HPX_ATTRIBUTE((nonnull(__VA_ARGS__)))
#define HPX_WEAK             HPX_ATTRIBUTE((weak))
#define HPX_CONSTRUCTOR      HPX_ATTRIBUTE((constructor))
#define HPX_DESTRUCTOR       HPX_ATTRIBUTE((destructor))
#define HPX_PRINTF(f, s)     HPX_ATTRIBUTE((format (printf, f, s)))
#define HPX_PUBLIC           HPX_ATTRIBUTE((visibility("default")))

// Attribute `returns_nonnull' was valid as of gcc 4.9.
#if (GCC_VERSION >= 40900)
# define HPX_RETURNS_NON_NULL HPX_ATTRIBUTE((returns_nonnull))
#else
# define HPX_RETURNS_NON_NULL
#endif // GNUC >= 4.9

#ifdef __clang__
# define HPX_NO_OPTIMIZE HPX_ATTRIBUTE((optnone))
#else
# define HPX_NO_OPTIMIZE HPX_OPTIMIZE("O0")
#endif

#ifdef __cplusplus
}
#endif

#endif // HPX_ATTRIBUTES_H
