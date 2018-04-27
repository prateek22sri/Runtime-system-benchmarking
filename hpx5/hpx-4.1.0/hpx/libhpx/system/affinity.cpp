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
# include "config.h"
#endif

#include <string.h>
#include <unistd.h>
#include <libhpx/debug.h>
#include <libhpx/libhpx.h>
#include <libhpx/locality.h>
#include <libhpx/system.h>
#include <libhpx/Topology.h>
#include <hwloc.h>

int system_set_worker_affinity(int id, libhpx_thread_affinity_t policy) {
  int resource;
  int cpu = (id % here->topology->ncpus);
  switch (policy) {
   case HPX_THREAD_AFFINITY_NUMA:
     resource = here->topology->cpu_to_numa[cpu];
     break;
   case HPX_THREAD_AFFINITY_CORE:
     resource = here->topology->cpu_to_core[cpu];
     break;
   case HPX_THREAD_AFFINITY_HWTHREAD:
     resource = cpu;
     break;
   case HPX_THREAD_AFFINITY_DEFAULT:
   case HPX_THREAD_AFFINITY_NONE:
     resource = -1;
     break;
   default:
     log_error("unknown thread affinity policy\n");
     return LIBHPX_ERROR;
  }

  // if we didn't find a valid cpuset, we ignore affinity.
  if (resource < 0) {
    return LIBHPX_OK;
  }

  libhpx_hwloc_cpuset_t cpuset = here->topology->cpu_affinity_map[resource];
  return  libhpx_hwloc_set_cpubind(here->topology->hwloc_topology,
                            cpuset, LIBHPX_HWLOC_CPUBIND_THREAD);
}

/// Return the weight of the bitmap that represents the CPUs we are
///  allowed to run on. This bitmap is set in libhpx/system/topology.c.
int system_get_available_cores(void) {
  return libhpx_hwloc_bitmap_weight(here->topology->allowed_cpus);
}
