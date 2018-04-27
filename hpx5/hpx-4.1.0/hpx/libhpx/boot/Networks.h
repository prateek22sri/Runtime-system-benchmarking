// ==================================================================-*- C++ -*-
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

#ifndef LIBHPX_BOOT_NETWORKS_H
#define LIBHPX_BOOT_NETWORKS_H

#include "libhpx/boot/Network.h"
#include "libhpx/config.h"

#ifdef HAVE_MPI
#include <mpi.h>
#endif

namespace libhpx {
namespace boot {
class SMP final : public Network {
 public:
  SMP();
  ~SMP();

  libhpx_boot_t type() const {
    return HPX_BOOT_SMP;
  }

  void barrier() const {
  }

  void allgather(const void* src, void* dest, int n) const;
  void alltoall(void * dest, const void * src, int n, int stride) const;
};

#ifdef HAVE_MPI
class MPI final : public Network {
  static constexpr int THREAD_LEVEL = MPI_THREAD_SERIALIZED;

 public:
  MPI();
  ~MPI();

  libhpx_boot_t type() const {
    return HPX_BOOT_MPI;
  }

  [[ noreturn ]] void abort() const;
  void barrier() const;
  void allgather(const void* src, void* dest, int n) const;
  void alltoall(void * dest, const void * src, int n, int stride) const;

 private:
  void init();

  MPI_Comm comm_;
  int finalizeMPI_;
};
#endif

#ifdef HAVE_PMI
class PMI final : public Network {
 public:
  PMI();
  ~PMI();

  libhpx_boot_t type() const {
    return HPX_BOOT_PMI;
  }

  [[ noreturn ]] void abort() const;
  void barrier() const;
  void allgather(const void* src, void* dest, int n) const;
  void alltoall(void * dest, const void * src, int n, int stride) const;

 private:
  static void encode(const void *src, size_t slen, char *dst, size_t *dlen); 
  static void decode(const char *src, size_t slen, void *dst, size_t dlen) ;
  static void putBuffer(char *kvs, int rank, void *buffer, size_t len);
  static void getBuffer(char *kvs, int rank, void *buffer, size_t len);
};
#endif
}
}

#endif // LIBHPX_BOOT_NETWORKS_H
