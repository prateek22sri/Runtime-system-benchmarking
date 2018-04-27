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

#include "hpcg.h"

void set_swap_lco(int n,Domain *domain)
{
  hpx_lco_set(domain->lcos.exchange_swap[n], 0, NULL, HPX_NULL, HPX_NULL);

  return;
}
