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

#ifndef LIBHPX_INSTRUMENTATION_FILE_H
#define LIBHPX_INSTRUMENTATION_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

// Output the header for the trace file.
size_t write_trace_header(void *base, int type, int event_id, int worker_id);

#ifdef __cplusplus
}
#endif

#endif // LIBHPX_INSTRUMENTATION_FILE_H
