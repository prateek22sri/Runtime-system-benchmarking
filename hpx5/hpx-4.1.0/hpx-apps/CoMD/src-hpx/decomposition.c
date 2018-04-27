// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2016-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

/*
Copyright (c) 2013, Los Alamos National Security, LLC
All rights reserved.

Copyright 2013. Los Alamos National Security, LLC. This software was produced 
under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National 
Laboratory (LANL), which is operated by Los Alamos National Security, LLC for 
the U.S. Department of Energy. The U.S. Government has rights to use, reproduce, 
and distribute this software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL 
SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY 
FOR THE USE OF THIS SOFTWARE.  If software is modified to produce derivative 
works, such modified software should be clearly marked, so as not to confuse 
it with the version available from LANL.

Additionally, redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

* Neither the name of Los Alamos National Security, LLC, Los Alamos National 
Laboratory, LANL, the U.S. Government, nor the names of its contributors may be
 used to endorse or promote products derived from this software without specific
 prior written permission.

THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

/// \file
/// Parallel domain decomposition.  This version of CoMD uses a simple
/// spatial Cartesian domain decomposition.  The simulation box is
/// divided into equal size bricks by a grid that is xproc by yproc by
/// zproc in size.

#include "decomposition.h"

#include <assert.h>

#include "memUtils.h"
#include <hpx/hpx.h>

#include "parallel_hpx.h"
/// \param [in] xproc x-size of domain decomposition grid.
/// \param [in] yproc y-size of domain decomposition grid.
/// \param [in] zproc z-size of domain decomposition grid.
/// \param [in] globalExtent Size of the simulation domain (in Angstroms).
Domain* initDecomposition(int xproc, int yproc, int zproc, real3 globalExtent) {

  assert(xproc * yproc * zproc == hpx_get_num_ranks());

  Domain* dd = comdMalloc(sizeof(Domain));
  dd->procGrid[0] = xproc;
  dd->procGrid[1] = yproc;
  dd->procGrid[2] = zproc;
  // calculate grid coordinates i,j,k for this processor
  int myRank = hpx_get_my_rank();
  dd->procCoord[0] = myRank % dd->procGrid[0];
  myRank /= dd->procGrid[0];
  dd->procCoord[1] = myRank % dd->procGrid[1];
  dd->procCoord[2] = myRank / dd->procGrid[1];

  // initialialize global bounds
  for (int i = 0; i < 3; i++) {
    dd->globalMin[i] = 0;
    dd->globalMax[i] = globalExtent[i];
    dd->globalExtent[i] = dd->globalMax[i] - dd->globalMin[i];
  }

  // initialize local bounds on this processor
  for (int i = 0; i < 3; i++) {
    dd->localExtent[i] = dd->globalExtent[i] / dd->procGrid[i];
    dd->localMin[i] = dd->globalMin[i] + dd->procCoord[i] * dd->localExtent[i];
    dd->localMax[i] = dd->globalMin[i]
        + (dd->procCoord[i] + 1) * dd->localExtent[i];
  }

  return dd;
}

/// \details
/// Calculates the rank of the processor with grid coordinates
/// (ix+dix, iy+diy, iz+diz) where (ix, iy, iz) are the grid coordinates
/// of the local rank.  Assumes periodic boundary conditions.  The
/// deltas cannot be smaller than -procGrid[ii].
int processorNum(Domain* domain, int dix, int diy, int diz) {
  const int* procCoord = domain->procCoord; // alias
  const int* procGrid = domain->procGrid;  // alias
  int ix = (procCoord[0] + dix + procGrid[0]) % procGrid[0];
  int iy = (procCoord[1] + diy + procGrid[1]) % procGrid[1];
  int iz = (procCoord[2] + diz + procGrid[2]) % procGrid[2];

  return ix + procGrid[0] * (iy + procGrid[1] * iz);
}
