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

#ifndef LIBHPX_PROCESS_BITMAP_H
#define LIBHPX_PROCESS_BITMAP_H

#include "libhpx/util/Aligned.h"
#include <climits>
#include <cstdint>
#include <atomic>

/// Credit-Recovery for detecting termination using a variant of
/// Mattern's credit-recovery scheme as described in:
/// "Global Quiescence Detection Based On Credit Distribution and
/// Recovery".
namespace libhpx {
namespace process {

/// The bitmap structure used for credit tracking.
///
/// This is a sparse bitmap. We have a top-level page of pointers to pages of
/// bits. The top-level page is fixed size, holding as many pointers as
/// possible.
class Bitmap : public util::Aligned<HPX_CACHELINE_SIZE> {
  static constexpr auto ACQUIRE = std::memory_order_acquire;
  static constexpr auto RELEASE = std::memory_order_release;
  static constexpr auto RELAXED = std::memory_order_relaxed;
  static constexpr auto ACQ_REL = std::memory_order_acq_rel;

  class Page;
  static constexpr unsigned Pages = HPX_PAGE_SIZE / sizeof(std::atomic<Page*>);

 public:
  typedef uint64_t Word;

  Bitmap() // : pages_()      Workaround for icc ICE
  {
    pages_[0].store(new Page(), RELEASE);
    for (unsigned i = 1; i < Pages; ++i) {
      pages_[i].store(nullptr, RELEASE);
    }
  }

  ~Bitmap() {
    for (unsigned i = 0; i < Pages; ++i) {
      delete pages_[i].load(ACQUIRE);
    }
  }

  /// Compute the page and offset and forward to the internal handler.
  Word addAndTest(int64_t i) {
    const unsigned page = (i - 1) / Pages;
    const unsigned offset = (i - 1) % Pages;
    return addBit(page, offset);
  }

 private:
  static constexpr unsigned Words = HPX_PAGE_SIZE / sizeof(std::atomic<Word>);
  static constexpr unsigned Bits = sizeof(Word) * CHAR_BIT;

  class Page : public util::Aligned<HPX_CACHELINE_SIZE> {
   public:
    Word baseWord() const {
      return words_[0].load(ACQUIRE);
    }

    /// Compute the word and bit and forward to the internal handler.
    bool addBit(const unsigned offset) {
      const unsigned word = offset / sizeof(Word);
      const unsigned bit = (Bits - 1) - offset % sizeof(Word);
      return addBit(word, bit);
    }

   private:
    bool addBit(const unsigned word, const unsigned bit);

    /// A page of atomic words.
    std::atomic<Word> words_[Words];
  };

  /// Pages are lazily allocated.
  Page* getPage(unsigned i);

  /// Add a bit on a page. This will recursively add bits to lower pages as
  /// necessary.
  Word addBit(const unsigned page, const unsigned offset);

  /// The top-level array of page pointers.
  std::atomic<Page*> pages_[Pages];

  static_assert(sizeof(Page) == HPX_PAGE_SIZE, "Unexpected bit packing.\n");
  static_assert(sizeof(Word) == sizeof(std::atomic<Word>), "Unexpected atomic size\n");
  static_assert(sizeof(Page*) == sizeof(std::atomic<Page*>), "Unexpected atomic size\n");
};

static_assert(sizeof(Bitmap) == HPX_PAGE_SIZE, "Unexpected bit packing.\n");
} // namespace process
} // namespace libhpx

#endif // LIBHPX_PROCESS_BITMAP_H
