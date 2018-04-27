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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Networks.h"
#include "libhpx/debug.h"

namespace {
using MPINetwork = libhpx::boot::MPI;
}

MPINetwork::MPI() : libhpx::boot::Network(), comm_(), finalizeMPI_(0)
{
  int init;
  if (MPI_Initialized(&init)) {
    throw log_error("mpi initialization failed\n");
  }

  if (!init) {
    int level = MPI_THREAD_SINGLE;
    if (MPI_Init_thread(NULL, NULL, THREAD_LEVEL, &level)) {
      throw log_error("mpi initialization failed\n");
    }

    finalizeMPI_ = 1;

    if (level != THREAD_LEVEL) {
      throw log_error("MPI thread level failed requested %d, received %d.\n",
                      THREAD_LEVEL, level);
    }
  }

  if (MPI_Comm_dup(MPI_COMM_WORLD, &comm_)) {
    throw log_error("mpi communicator duplication failed\n");
  }


  if (MPI_Comm_rank(comm_, &rank_)) {
    throw log_error("Could not get rank\n");
  }

  if (MPI_Comm_size(comm_, &nRanks_)) {
    throw log_error("Could not get the number of ranks\n");
  }

}

MPINetwork::~MPI()
{
  if (finalizeMPI_) {
    MPI_Finalize();
  }
}

void
MPINetwork::barrier() const
{
  MPI_Barrier(comm_);
}

void
MPINetwork::allgather(const void* src, void* dest, int n) const
{
  if (auto e = MPI_Allgather((char*)src, n, MPI_BYTE, dest, n, MPI_BYTE, comm_))
  {
    dbg_error("failed MPI_Allgather %d.\n", e);
  }
}

void
MPINetwork::alltoall(void* dest, const void* src, int n, int stride) const
{
  int *counts = new int[nRanks_];
  int *offsets = new int[nRanks_];
  for (int i = 0, e = nRanks_; i < e; ++i) {
    counts[i] = n;
    offsets[i] = i * stride;
  }
  if (MPI_Alltoallv((void*)src, counts, offsets, MPI_BYTE, dest, counts,
                    offsets, MPI_BYTE, comm_)) {
    dbg_error("MPI_Alltoallv failed at bootstrap\n");
  }
  delete [] offsets;
  delete [] counts;
}

void
MPINetwork::abort() const
{
  MPI_Abort(comm_, -6);
  unreachable();
}
