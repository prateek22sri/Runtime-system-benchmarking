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

#ifndef LIBHPX_ACTION_EXIT_H
#define LIBHPX_ACTION_EXIT_H

/// @file libhpx/action/exit.h
///
/// Declare two exit handlers that are used in the parcel management vtables.

/// Forward declare the parcel to avoid including hpx.h here.
/// @{
struct hpx_parcel;
/// @}

void exit_action(const void *obj, struct hpx_parcel *p);
void exit_pinned_action(const void *obj, struct hpx_parcel *p);

#endif // LIBHPX_ACTION_EXIT_H
