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

#include "libhpx/Topology.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include "libhpx/locality.h"
#include "libhpx/system.h"
#include "libhpx/util/Env.h"
#include <alloca.h>
#include <string.h>
#include <unistd.h>
#include <hwloc.h>

namespace {
  using namespace libhpx;
}

static int _get_resources_by_affinity(Topology *topology,
                                      libhpx_thread_affinity_t policy) {
  int n = 0;
  switch (policy) {
   default:
     log_error("unknown thread affinity policy\n");
   case HPX_THREAD_AFFINITY_DEFAULT:
   case HPX_THREAD_AFFINITY_NONE:
     return 0;
   case HPX_THREAD_AFFINITY_NUMA:
     n = topology->nnodes;
     break;
   case HPX_THREAD_AFFINITY_CORE:
     n = topology->ncores;
     break;
   case HPX_THREAD_AFFINITY_HWTHREAD:
     n = topology->ncpus;
     break;
  }
  return n;
}

static libhpx_hwloc_cpuset_t *_cpu_affinity_map_new(Topology *topology,
                                             libhpx_thread_affinity_t policy) {
  int resources = _get_resources_by_affinity(topology, policy);
  if (!resources) {
    return NULL;
  }
  libhpx_hwloc_cpuset_t *cpu_affinity_map = new libhpx_hwloc_cpuset_t[resources];

  for (int r = 0; r < resources; ++r) {
    libhpx_hwloc_cpuset_t cpuset = cpu_affinity_map[r] = libhpx_hwloc_bitmap_alloc();
    switch (policy) {
     case HPX_THREAD_AFFINITY_DEFAULT:
     case HPX_THREAD_AFFINITY_NUMA:
       for (int i = 0; i < topology->ncpus; ++i) {
         if (r == topology->cpu_to_numa[i]) {
           libhpx_hwloc_bitmap_set(cpuset, i);
         }
       }
       break;
     case HPX_THREAD_AFFINITY_CORE:
       for (int i = 0; i < topology->ncpus; ++i) {
         if (r == topology->cpu_to_core[i]) {
           libhpx_hwloc_bitmap_set(cpuset, i);
         }
       }
       break;
     case HPX_THREAD_AFFINITY_HWTHREAD:
       libhpx_hwloc_bitmap_set(cpuset, r);
       libhpx_hwloc_bitmap_singlify(cpuset);
       break;
     case HPX_THREAD_AFFINITY_NONE:
       libhpx_hwloc_bitmap_set_range(cpuset, 0, topology->ncpus-1);
       break;
     default:
       log_error("unknown thread affinity policy\n");
       return NULL;
    }
  }
  return cpu_affinity_map;
}

static void _cpu_affinity_map_delete(Topology *topology) {
  int resources = _get_resources_by_affinity(topology, here->config->thread_affinity);
  for (int r = 0; r < resources; ++r) {
    libhpx_hwloc_bitmap_free(topology->cpu_affinity_map[r]);
  }
  delete [] topology->cpu_affinity_map;
}

Topology::~Topology() {
  if (cpus) {
    delete [] cpus;
    cpus = NULL;
  }

  if (numa_nodes) {
    delete [] numa_nodes;
    numa_nodes = NULL;
  }

  if (cpu_to_core) {
    delete [] cpu_to_core;
    cpu_to_core = NULL;
  }

  if (cpu_to_numa) {
    delete [] cpu_to_numa;
    cpu_to_numa = NULL;
  }

  if (numa_to_cpus) {
    for (int i = 0; i < nnodes; ++i) {
      if (numa_to_cpus[i]) {
        delete [] numa_to_cpus[i];
      }
    }
    delete [] numa_to_cpus;
    numa_to_cpus = NULL;
  }

  if (allowed_cpus) {
    libhpx_hwloc_bitmap_free(allowed_cpus);
    allowed_cpus = NULL;
  }

  if (cpu_affinity_map) {
    _cpu_affinity_map_delete(this);
    cpu_affinity_map  = NULL;
  }

  libhpx_hwloc_topology_destroy(hwloc_topology);
}

Topology::Topology(const config_t *config)
  : ncpus(0),
    ncores(0),
    nnodes(0),
    numa_nodes(nullptr),
    cpus_per_node(0),
    cpu_to_core(0),
    cpu_to_numa(nullptr),
    numa_to_cpus(0),
    cpu_affinity_map(nullptr)
{
  // Initialize the hwloc infrastructure.
  libhpx_hwloc_topology_t *hwloc = &hwloc_topology;
  if (libhpx_hwloc_topology_init(hwloc)) {
    log_error("failed to initialize HWLOC topology.\n");
    return;
  }

  if (libhpx_hwloc_topology_load(*hwloc)) {
    log_error("failed to load HWLOC topology.\n");
    return;
  }

  // Query the hwloc object for cpus, cores, and numa nodes---"fix" nnodes if
  // hwloc returns something unpleasant
  ncpus = libhpx_hwloc_get_nbobjs_by_type(*hwloc, LIBHPX_HWLOC_OBJ_PU);
  ncores = libhpx_hwloc_get_nbobjs_by_type(*hwloc, LIBHPX_HWLOC_OBJ_CORE);
  nnodes = libhpx_hwloc_get_nbobjs_by_type(*hwloc, LIBHPX_HWLOC_OBJ_NUMANODE);
  nnodes = (nnodes > 0) ? nnodes : 1;
  cpus_per_node = ncpus / nnodes;

  // Allocate our secondary arrays.
  allowed_cpus = libhpx_hwloc_bitmap_alloc();
  if (!allowed_cpus) {
    log_error("failed to allocate memory for cpuset.\n");
    return;
  }

  cpus = new libhpx_hwloc_obj_t[ncpus]();
  if (!cpus) {
    log_error("failed to allocate memory for cpu objects.\n");
    return;
  }

  cpu_to_core = new int[ncpus]();
  if (!cpu_to_core) {
    log_error("failed to allocate memory for the core map.\n");
    return;
  }

  cpu_to_numa = new int[ncpus]();
  if (!cpu_to_numa) {
    log_error("failed to allocate memory for the NUMA map.\n");
    return;
  }

  numa_nodes = new libhpx_hwloc_obj_t[nnodes]();
  if (!numa_nodes) {
    log_error("failed to allocate memory for numa node objects.\n");
    return;
  }

  numa_to_cpus = new int*[nnodes]();
  if (!numa_to_cpus) {
    log_error("failed to allocate memory for the reverse NUMA map.\n");
    return;
  }

  for (int i = 0, e = nnodes; i < e; ++i) {
    numa_to_cpus[i] = new int[cpus_per_node]();
    if (!numa_to_cpus[i]) {
      log_error("failed to allocate memory for the reverse NUMA map.\n");
      return;
    }
  }

  // Detect how many CPUs we can run on based on affinity or environment.  On
  // aprun platforms, we look at the ALPS depth to figure out how many CPUs to
  // use, otherwise we try and use hwloc to figure out what the process mask is
  // set to, otherwise we just bail out and use all CPUs.
  int cores = 0;
  try {
    cores = libhpx::util::getEnv<int>("ALPS_APP_DEPTH");
  }
  catch (const libhpx::util::NotFound&) {
    // Check for the SLURM launcher.
    try {
      cores = libhpx::util::getEnv<int>("SLURM_CPUS_PER_TASK");
    }
    catch (const libhpx::util::NotFound&) { }
  }

  if (cores > 0) {
    libhpx_hwloc_bitmap_set_range(allowed_cpus, 0, cores - 1);
  }
  else if (libhpx_hwloc_get_cpubind(*hwloc, allowed_cpus, LIBHPX_HWLOC_CPUBIND_PROCESS))
  {
    libhpx_hwloc_bitmap_set_range(allowed_cpus, 0, ncpus - 1);
  }

  // We want to be able to map from a numa domain to its associated cpus, but we
  // are going to iterate through the cpus in an undefined order. This array
  // keeps track of the "next" index for each numa node array, so that we can
  // insert into the right place.
  int numa_to_cpus_next[nnodes];
  memset(numa_to_cpus_next, 0, sizeof(numa_to_cpus_next));

  libhpx_hwloc_obj_t cpu = NULL;
  libhpx_hwloc_obj_t core = NULL;
  libhpx_hwloc_obj_t numa_node = NULL;
  for (int i = 0, e = ncpus; i < e; ++i) {
    // get the hwloc tree nodes for the cpu, core, and numa node
    cpu = libhpx_hwloc_get_next_obj_by_type(*hwloc, LIBHPX_HWLOC_OBJ_PU, cpu);
    core = libhpx_hwloc_get_ancestor_obj_by_type(*hwloc, LIBHPX_HWLOC_OBJ_CORE, cpu);
    numa_node = libhpx_hwloc_get_ancestor_obj_by_type(*hwloc, LIBHPX_HWLOC_OBJ_NUMANODE, cpu);

    // get integer indexes for the cpu, core, and numa node
    int index = cpu->os_index;
    int core_index = (core) ? core->os_index : 0;
    #if defined(__PPC64__)
      int numa_index = (numa_node) ? numa_node->logical_index : 0;
    #else
      int numa_index = (numa_node) ? numa_node->os_index : 0;
    #endif

    // record our hwloc nodes so that we can get them quickly during queries
    cpus[index] = cpu;
    numa_nodes[numa_index] = numa_node;

    // record our core and numa indices for quick query
    cpu_to_core[index] = core_index;
    cpu_to_numa[index] = numa_index;

    // and record this cpu in the correct numa set
    int next_cpu_index = numa_to_cpus_next[numa_index]++;
    numa_to_cpus[numa_index][next_cpu_index] = index;
  }

  // generate the CPU affinity map
  cpu_affinity_map = _cpu_affinity_map_new(this, config->thread_affinity);
}
