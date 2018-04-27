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

#ifndef LIBHPX_PERCOLATION_H
#define LIBHPX_PERCOLATION_H

/// @file include/libhpx/percolation.h
/// @brief Exports methods and structures related to percolation.
///

#ifdef __cplusplus
extern "C" {
#endif

#include "hpx/attributes.h"
#include "hpx/hpx.h"
#include "libhpx/config.h"

typedef struct {
  const char *(*id)(void);
  void (*deallocate)(void*);
  void *(*prepare)(const void*, const char *, const char *);
  int (*execute)(const void*, void *, int, void **, size_t *);
  void (*destroy)(const void*, void *);
} percolation_t;

percolation_t *percolation_new_opencl(void);
percolation_t *percolation_new(void);

/// Percolation  interface.
/// @{

// /// Delete a percolation object.
void percolation_deallocate(void *obj);

/// Percolate an action to be executed later on a device.
void *percolation_prepare(const void *obj, const char *key,
                          const char *kernel);

/// Execute a percolation object on an available device.
int percolation_execute(const void *obj, void *o, int n, void *vargs[],
                        size_t sizes[]);

/// Destroy or release the state associated with a percolation object.
void percolation_destroy(const void *obj, void *o);

/// An action to launch OpenCL kernels.
extern HPX_ACTION_DECL(percolation_execute_action);

/// The action handler that launches OpenCL kernels.
int percolation_execute_handler(int nargs, void *vargs[],
        size_t sizes[]);
/// @}


#ifdef __cplusplus
}
#endif

#endif // LIBHPX_PERCOLATION_H
