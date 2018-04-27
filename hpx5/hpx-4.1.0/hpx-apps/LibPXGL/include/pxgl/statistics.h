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

#ifndef PXGL_STAT_H
#define PXGL_STAT_H

#include "defs.h"

#include "hpx/hpx.h"
#include "adjacency_list.h"

#define PRINT_STATS(lbl, israte)				   \
  do {								   \
    printf ("min_%s: %20.17e\n", lbl, stats[0]);		   \
    printf ("firstquartile_%s: %20.17e\n", lbl, stats[1]);	   \
    printf ("median_%s: %20.17e\n", lbl, stats[2]);		   \
    printf ("thirdquartile_%s: %20.17e\n", lbl, stats[3]);	   \
    printf ("max_%s: %20.17e\n", lbl, stats[4]);		   \
    if (!israte) {						   \
      printf ("mean_%s: %20.17e\n", lbl, stats[5]);		   \
      printf ("stddev_%s: %20.17e\n", lbl, stats[6]);		   \
      } else {							   \
      printf ("harmonic_mean_%s: %20.17e\n", lbl, stats[7]);	   \
      printf ("harmonic_stddev_%s: %20.17e\n", lbl, stats[8]);	   \
    }								   \
  } while (0)



extern void statistics (double *out, double *data, sssp_int_t n);


#endif
