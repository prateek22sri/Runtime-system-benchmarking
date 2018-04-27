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

#include "emulate_pwc.h"
#include "libhpx/action.h"
#include "libhpx/parcel.h"
#include "libhpx/Worker.h"
#include <cstring>

namespace {
using libhpx::self;
}

/// Emulate a put-with-completion operation.
///
/// This will copy the data buffer into the correct place, and then continue to
/// the completion handler.
int isir_emulate_pwc_handler(void *to, const void *buffer, size_t n) {
  std::memcpy(to, buffer, n);
  return HPX_SUCCESS;
}
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED | HPX_MARSHALLED, isir_emulate_pwc,
              isir_emulate_pwc_handler, HPX_POINTER, HPX_POINTER, HPX_SIZE_T);

typedef struct {
  void *lva;
  char bytes[];
} _gwc_reply_args_t;

/// The reply half of a get-with-completion.
///
static int _gwc_reply_handler(const _gwc_reply_args_t *args, size_t n) {
  std::memcpy(args->lva, args->bytes, n - sizeof(*args));
  return HPX_SUCCESS;
}
static LIBHPX_ACTION(HPX_TASK, HPX_MARSHALLED, _gwc_reply,
                     _gwc_reply_handler, HPX_POINTER, HPX_SIZE_T);

/// Emulate the remote side of a get-with-completion.
///
/// This means copying n bytes from the local target to @p to through a parcel,
/// and then signaling whatever the lsync should be over there.
///
/// NB: once the PINNED changes get incorporated, this can be an interrupt.
static int _gwc_request_handler(void *from, size_t n, hpx_addr_t to, void *lva)
{
  _gwc_reply_args_t *args = nullptr;
  hpx_parcel_t *p = hpx_parcel_acquire(nullptr, sizeof(*args) + n);
  p->target = to;
  p->action = _gwc_reply;

  // *take* the current continuation
  hpx_parcel_t *current = self->getCurrentParcel();
  p->c_target = current->c_target;
  p->c_action = current->c_action;
  current->c_target = HPX_NULL;
  current->c_action = HPX_ACTION_NULL;

  args = static_cast<_gwc_reply_args_t*>(hpx_parcel_get_data(p));
  args->lva = lva;
  std::memcpy(args->bytes, from, n);
  hpx_parcel_send(p, HPX_NULL);
  return HPX_SUCCESS;
}
LIBHPX_ACTION(HPX_DEFAULT, HPX_PINNED, isir_emulate_gwc, _gwc_request_handler,
              HPX_POINTER, HPX_SIZE_T, HPX_ADDR, HPX_POINTER);
