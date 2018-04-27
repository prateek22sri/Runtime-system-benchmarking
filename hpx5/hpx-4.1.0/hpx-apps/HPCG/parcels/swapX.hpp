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

#ifndef SWAPX_HPP
#define SWAPX_HPP

#include "SparseMatrix.hpp"
#include "Vector.hpp"
#include "hpcg.h"

void swapX(const SparseMatrix &A, Vector & x,int generation,Domain *domain);

void set_swap_lco(int n,Domain *domain);

#endif 
