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

#include "ISendBuffer.h"
#include "parcel_utils.h"
#include "libhpx/debug.h"
#include "hpx/builtins.h"
#include <exception>
#include <memory>

namespace {
using libhpx::network::isir::ISendBuffer;
constexpr unsigned ISIR_TWIN_INC = 10;
}

/// Compute the buffer index of an abstract index.
///
/// @param           id The isend id.
/// @param            n The size of the buffer (must be 2^k).
///
/// @returns            The index in the buffer for this abstract index.
static unsigned
_index_of(unsigned long id, unsigned n)
{
  return (id & (n - 1));
}

/// Figure out what tag I'm supposed to use for a particular payload size.
///
/// @param       payload The payload size.
///
/// @returns             The correct tag.
int
ISendBuffer::PayloadSizeToTag(unsigned payload)
{
  unsigned parcel_size = payload + sizeof(hpx_parcel_t);
  return ceil_div_32(parcel_size, HPX_CACHELINE_SIZE);
}

/// Re-size an isend buffer to the requested size.
///
/// Buffer sizes can only be increased in the current implementation. The size
/// must be a power of 2.
///
/// @param         size The new size.
void
ISendBuffer::reserve(unsigned size)
{
  dbg_assert_str(size >= size_, "cannot shrink send buffer\n");

  if (size == size_) {
    return;
  }

  // start by resizing the buffers
  unsigned oldsize = size_;
  size_ = size;
  requests_ = static_cast<Request*>(realloc(requests_, size * sizeof(Request)));
  records_ = static_cast<Record*>(realloc(records_, size * sizeof(Record)));

  if (!requests_ || !records_) {
    log_error("failed to resize a send buffer from %u to %u\n", oldsize, size);
    throw std::exception();
  }

  if (max_ == min_) {
    log_net("resized a send buffer from %u to %u\n", oldsize, size);
    return;
  }

  // Resizing the buffer changes where our index mapping is, we need to move
  // data around in the arrays. We do that by memcpy-ing either the prefix or
  // suffix of a wrapped buffer into the new position. After resizing the buffer
  // should never be wrapped.
  unsigned min = _index_of(min_, oldsize);
  unsigned max = _index_of(max_, oldsize);
  unsigned prefix = (min < max) ? max - min : oldsize - min;
  unsigned suffix = (min < max) ? 0 : max;

  unsigned nmin = _index_of(min_, size);
  unsigned nmax = _index_of(max_, size);

  // This code is slightly tricky. We only need to move one of the ranges,
  // either [min, oldsize) or [0, max). We determine which range we need to move
  // by seeing if the min or max index is different in the new buffer, and then
  // copying the appropriate bytes of the requests and records arrays to the
  // right place in the new buffer.
  if (min == nmin) {
    assert(max != nmax);
    assert(min + prefix == nmax - suffix);
    std::copy(&requests_[0], &requests_[suffix], &requests_[min + prefix]);
    std::copy(&records_[0], &records_[suffix], &records_[min + prefix]);
  }
  else if (max == nmax) {
    assert(0 < prefix);
    assert(nmin + prefix <= size);
    std::copy(&requests_[min], &requests_[min + prefix], &requests_[nmin]);
    std::copy(&records_[min], &records_[min + prefix], &records_[nmin]);
  }
  else {
    log_error("unexpected shift in isend buffer reserve\n");
    throw std::exception();
  }

  log_net("resized a send buffer from %u to %u\n", oldsize, size);
}

/// Start an isend operation.
///
/// A precondition of this function is that there must be a valid entry in the
/// buffer that is not yet active.
///
/// @param           id The isend to start.
void
ISendBuffer::start(unsigned long id)
{
  unsigned i = _index_of(id, size_);
  hpx_parcel_t *p = records_[i].parcel;
  void *from = isir_network_offset(p);
  unsigned to = gas_.ownerOf(p->target);
  unsigned n = payload_size_to_isir_bytes(p->size);
  int tag = PayloadSizeToTag(p->size);
  log_net("starting a parcel send: tag %d, %d bytes\n", tag, n);
  requests_[i] = xport_.isend(to, from, n, tag);
}

/// Start as many isend operations as we can.
///
/// @returns            The number of sends that remain unstarted.
unsigned long
ISendBuffer::startAll()
{
  unsigned long end = (limit_) ? std::min(min_ + limit_, max_) : max_;
  while (active_ < end) {
    start(active_++);
  }
  return (max_ - active_);
}

/// Test a contiguous range of the buffer.
///
/// This performs a single MPI_Testsome() on a range of requests covering
/// [i, i + n), and calls _finish() for each request that is completed.
///
/// @param            i The physical index at which the range starts.
/// @param            n The number of sends to test.
/// @param[out]     out The output.
/// @param[out]   ssync Any synchronization parcels that we completed.
///
/// @returns            The number of completed requests in this range.
unsigned
ISendBuffer::testRange(unsigned i, unsigned n, int* out, hpx_parcel_t **ssync)
{
  assert(i + n <= size_);

  if (n == 0) return 0;

  int cnt = xport_.Testsome(n, &requests_[i], out);
  assert(0 <= cnt);

  for (int j = 0; j < cnt; ++j) {
    out[j] += i;
    unsigned k = out[j];
    assert(i <= k && k < i + n);

    // handle each of the completed requests
    parcel_delete(records_[k].parcel);
    while (hpx_parcel_t *p = parcel_stack_pop(&records_[k].ssync)) {
      parcel_stack_push(ssync, p);
    }
  }

  return cnt;
}

/// Compact the buffer after testing.
///
/// @param            n The number of valid entries in @p out.
/// @param          out The entries that are completed.
void
ISendBuffer::compact(unsigned long n, const int out[])
{
  unsigned long m = active_ - min_;
  if (n == m) {
    log_net("bulk compaction of %lu sends in buffer\n", n);
    min_ += n;
    return;
  }

  // incremental compaction
  assert(n < INT_MAX / 2);
  for (int i = 0, e = n; i < e; ++i, ++min_) {
    unsigned min = _index_of(min_, size_);
    requests_[out[i]] = requests_[min];
    records_[out[i]] = records_[min];
  }

  log_net("incremental compaction of %lu sends in buffer\n", n);
}

/// Test all of the active isend operations.
///
/// The isend buffer is a standard circular buffer, so we need to test one or
/// two ranges, depending on if the buffer is currently wrapped.
///
/// @param[out]   ssync The collected synchronization parcels.
///
/// @returns            The number of sends completed.
unsigned long
ISendBuffer::testAll(hpx_parcel_t **ssync)
{
  unsigned i = _index_of(min_, size_);
  unsigned j = _index_of(active_, size_);

  // might have to test two ranges, [i, size) and [0, j), or just [i, j)
  bool wrapped = (j < i);
  unsigned n = (wrapped) ? size_ - i : j - i;
  unsigned m = (wrapped) ? j : 0;

  // limit how many requests we test
  n = std::min(n, twin_);
  m = std::min(m, (twin_ - n));

  std::unique_ptr<int[]> out(new int[n + m]);
  unsigned long total = 0;
  total += testRange(i, n, &out[0], ssync);
  total += testRange(0, m, &out[total], ssync);
  if (total) {
    compact(total, &out[0]);
  }
  if (total >= twin_) {
    twin_ += ISIR_TWIN_INC;
    log_net("increased test window to %u\n", twin_);
  }
  else if ((twin_ - total) > ISIR_TWIN_INC) {
    twin_ -= ISIR_TWIN_INC;
    log_net("decreased test window to %u\n", twin_);
  }
  DEBUG_IF (total) {
    log_net("tested %u sends, completed %lu\n", n+m, total);
  }
  return total;
}

void
ISendBuffer::cancel(unsigned long id, hpx_parcel_t **parcels)
{
  unsigned i = _index_of(id, size_);
  xport_.cancel(requests_[i]);
  parcel_stack_push(parcels, records_[i].parcel);
  while (hpx_parcel_t *p = parcel_stack_pop(&records_[i].ssync)) {
    parcel_stack_push(parcels, p);
  }
}

hpx_parcel_t *
ISendBuffer::cancelAll()
{
  hpx_parcel_t *p = nullptr;
  for (unsigned long id = min_, e = active_; id < e; ++id) {
    cancel(id, &p);
  }
  return p;
}

ISendBuffer::ISendBuffer(const config_t *cfg, GAS *gas, Transport &xport)
    : gas_(*gas),
      xport_(xport),
      limit_(cfg->isir_sendlimit),
      twin_(cfg->isir_testwindow),
      size_(0),
      min_(0),
      active_(0),
      max_(0),
      requests_(nullptr),
      records_(nullptr)
{
  reserve(64);
}

ISendBuffer::~ISendBuffer()
{
  hpx_parcel_t *stack = cancelAll();
  while (hpx_parcel_t *p = parcel_stack_pop(&stack)) {
    parcel_delete(p);
  }

  if (records_) {
    free(records_);
  }

  if (requests_) {
    free(requests_);
  }
}

void
ISendBuffer::append(hpx_parcel_t *p, hpx_parcel_t *ssync)
{
  unsigned max = _index_of(max_++, size_);
  records_[max].parcel = p;
  records_[max].ssync = ssync;
  if (size_ <= max_ - min_) {
    reserve(2 * size_);
  }
}

int
ISendBuffer::flush(hpx_parcel_t **ssync)
{
  int total = 0;
  while (min_ != max_) {
    total += progress(ssync);
  }
  return total;
}

int
ISendBuffer::progress(hpx_parcel_t **ssync)
{
  unsigned long m = testAll(ssync);
  unsigned long n = startAll(); (void)n;
  DEBUG_IF (m) {
    log_net("finished %lu sends\n", m);
  }
  DEBUG_IF (n) {
    log_net("failed to start %lu sends\n", n);
  }
  return m;
}
