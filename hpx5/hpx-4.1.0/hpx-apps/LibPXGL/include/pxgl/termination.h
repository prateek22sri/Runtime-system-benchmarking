// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#ifndef PXGL_TERMINATION_H
#define PXGL_TERMINATION_H

typedef enum { COUNT_TERMINATION,
               AND_LCO_TERMINATION,
               PROCESS_TERMINATION } termination_t;
void set_termination(termination_t);

#endif // LIBPXGL_TERMINATION_H
