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

#ifndef ACTIONS2_H
#define ACTIONS2_H

typedef struct {
  int  src;
  int n;
  int send_length;
  double buf[]; // inline, variable length buffer
} NodalArgs;

#endif
