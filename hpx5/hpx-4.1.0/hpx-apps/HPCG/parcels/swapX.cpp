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

#include "swapX.hpp"
#include <stdio.h>

void swapX(const SparseMatrix & A, Vector & x,int generation,Domain *domain)
{
  int n = (generation & 1) ? 1 : 0;
  set_swap_lco(n,domain);

  if ( n == 0 ) {
    domain->eX = &x;
    domain->eA = &A;
  } else {
    domain->fX = &x;
    domain->fA = &A;
  }

  return;
}
