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
#include <config.h>
#endif

#include "Networks.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include <pmi.h>
#include <cstdlib>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <cinttypes>
#include <cassert>

namespace {
  using libhpx::boot::PMI;
}

PMI::PMI() : libhpx::boot::Network()
{
  int init;
  PMI_Initialized(&init);
  assert(!init);

  int spawned;
  if (PMI_Init(&spawned) ) {
    throw log_error("unexpected PMI initialization\n");
  }

  if (PMI_Get_rank(&rank_)) {
    throw log_error("could not get rank\n");
  }

  if (PMI_Get_size(&nRanks_)) {
    throw log_error("could not get ranks\n");
  }
}

PMI::~PMI() 
{
  PMI_Finalize();
}

void
PMI::barrier() const 
{
  PMI_Barrier();
}

void 
PMI::abort() const
{
  PMI_Abort(-6, "HPX aborted.");
  unreachable();
}


/// ----------------------------------------------------------------------------
/// Base64 string encoding.
///
/// todo: Cray PMI does not allow characters such as /;=. We need to
/// make the encoding function account for that.
/// ----------------------------------------------------------------------------
void
PMI::encode(const void *src, size_t slen, char *dst, size_t *dlen) 
{
  assert ((dlen != NULL) || (dst != NULL));
  unsigned char *s = (unsigned char*)src;
  char *d = (char*)dst;
  char *p;

  *dlen = ((slen+3)/4)*6+1;
  uint64_t w;
  while (slen > 3) {
    w = *s++; w = (w << 8) | *s++; w = (w << 8) | *s++; w = (w << 8) | *s++;
    slen -= 4;
    p = stpcpy(d, l64a(htonl(w)));
    d = (char*)mempcpy(p, "......", 6-(p-d));
  }

  if (slen > 0) {
    w = *s++;
    if (--slen > 0) {
      w = (w << 8) | *s++;
      if (--slen > 0)
        w = (w << 8) | *s;
    }
    d = stpcpy(d, l64a(htonl(w)));
  }
  *d = '\0';
}

/// ----------------------------------------------------------------------------
/// Base64 string decoding. The length of source and destination
/// buffers is always unequal due to padding.
/// ----------------------------------------------------------------------------
void
PMI::decode(const char *src, size_t slen, void *dst, size_t dlen) 
{
  assert (dst != NULL);
  char *s = (char*)src;
  unsigned char *d = (unsigned char*)dst;

  assert(dlen >= (2*slen-11)/3);
  long w;
  for (int i = 0, j = 0; i < slen; j+=4, i+=6) {
    w = ntohl(a64l(s+i));
    d[j+3] = w & 0xFF;
    w = (w >> 8);
    d[j+2] = w & 0xFF;
    w = (w >> 8);
    d[j+1] = w & 0xFF;
    w = (w >> 8);
    d[j] = w & 0xFF;
  }
  // handle left-over?

}

/// ----------------------------------------------------------------------------
/// This publishes a buffer in PMI's key-value store. The buffer
/// is associated with our rank as its key. Since the value needs to
/// be a string buffer, it is first encoded using base64 encoding
/// before doing a KVS put.
/// ----------------------------------------------------------------------------
void
PMI::putBuffer(char *kvs, int rank, void *buffer, size_t len)
{
  int length;
  const int pmi_maxlen = 256;

  // allocate key
  int e = PMI_KVS_Get_key_length_max(&length);
  if (e != PMI_SUCCESS) {
    log_error("pmi: failed to get max key length.\n");
    length = pmi_maxlen;
  }
  char *key = new char[length];
  snprintf(key, length, "%d", rank);

  // allocate value
  e = PMI_KVS_Get_value_length_max(&length);
  if (e != PMI_SUCCESS) {
    delete [] key;
    log_error("pmi: failed to get max value length.\n");
    length = pmi_maxlen;
  }
  char *value = new char[length];
  encode(buffer, len, value, (size_t*)&length);

  // do the key-value pair exchange
  if ((PMI_KVS_Put(kvs, key, value)) != PMI_SUCCESS) {
    goto error;
  }

  if ((PMI_KVS_Commit(kvs)) != PMI_SUCCESS) {
    goto error;
  }

  PMI_Barrier();
  delete [] key;
  delete [] value;
  return;

error:
  delete [] key;
  delete [] value;
  dbg_error("pmi: failed to put buffer in PMI's KVS.\n");
}

/// ----------------------------------------------------------------------------
/// This retrieves a buffer from PMI's key-value store. The buffer is
/// base64 decoded before it is returned.
/// ----------------------------------------------------------------------------
void
PMI::getBuffer(char *kvs, int rank, void *buffer, size_t len)
{
  int length;

  // allocate key
  if (int e = PMI_KVS_Get_key_length_max(&length)) {
    dbg_error("pmi: failed to get max key length.\n");
  }
  char *key = new char[length];
  snprintf(key, length, "%d", rank);

  // allocate value
  if (int e = PMI_KVS_Get_value_length_max(&length)) {
    delete [] key;
    dbg_error("pmi: failed to get max value length.\n");
  }
  char *value = new char[length];
  if ((PMI_KVS_Get(kvs, key, value, length)) != PMI_SUCCESS)
    goto error;

  decode(value, strlen(value), buffer, len);

  delete [] key;
  delete [] value;
  return;

error:
  delete [] key;
  delete [] value;
  dbg_error("pmi: failed to put buffer in PMI's KVS.\n");
}


void
PMI::allgather(const void *cin, void *out, int n)  const
{
  void *in = (void*)cin;

#if HAVE_PMI_CRAY_EXT
  // nb: Cray PMI allgather does not guarantee process-rank
  // order. Here, we assume that the ordering is, at least,
  // deterministic for all allgather invocations.

  int *ranks = new int[nRanks_]();
  if (PMI_Allgather((void*)&rank_, ranks, sizeof(rank_))) {
    delete [] ranks;
    dbg_error("pmi: failed in PMI_Allgather.\n");
  }

  void *buf = malloc(sizeof(char) * n * nRanks_);
  assert(buf != NULL);
  if ((PMI_Allgather(in, buf, n)) != PMI_SUCCESS) {
    free(buf);
    delete [] ranks;
    dbg_error("pmi: failed in PMI_Allgather.\n");
  }

  for (int i = 0; i < nRanks_; i++) {
    memcpy((char*)out+(ranks[i]*n), (char*)buf+(i*n), n);
  }

  free(buf);
  delete [] ranks;
#else
  int length;

  // allocate name for the nidpid map exchange
  if (PMI_KVS_Get_name_length_max(&length)) {
    dbg_error("pmi: failed to get max name length.\n");
  }
  char *name = new char[length];
  if (PMI_KVS_Get_my_name(name, length)) {
    delete [] name;
    dbg_error("pmi: failed to get kvs name.\n");
  }

  putBuffer(name, nRanks_, (void*)in, n);

  for (int r = 0; r < nRanks_nranks; r++)
    getBuffer(name, r, (char*)out+(r*n), n);

#endif
}

void
PMI::alltoall(void *dest, const void *src, int n, int stride) const
{
  // Emulate alltoall with allgather for now.

  // Allocate a temporary buffer to allgather into
  int gather_bytes = nRanks_ * nRanks_ * stride;
  void *gather = malloc(gather_bytes);
  if (!gather) {
    dbg_error("could not allocate enough space for PMI alltoall emulation\n");
  }

  // Perform the allgather
  allgather(src, gather, nRanks_ * stride);

  // Copy out the data
  const char *from = static_cast<const char*>(gather);
  char *to = static_cast<char*>(dest);
  for (int i = 0, e = nRanks_; i < e; ++i) {
    int offset = (i * nRanks_ * stride) + (rank_ * stride);
    memcpy(to + (i * stride), from + offset, n);
  }

  free(gather);
}
