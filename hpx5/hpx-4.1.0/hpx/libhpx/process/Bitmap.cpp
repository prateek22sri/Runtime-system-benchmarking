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

#include "Bitmap.h"
#include <cassert>

namespace {
using libhpx::process::Bitmap;
}

Bitmap::Page*
Bitmap::getPage(const unsigned page)
{
  // try a simple read---this only fails on first access
  if (Page* p = pages_[page].load(ACQUIRE)) {
    return p;
  }

  // insert a new page, if it fails then `p` is given the value of the page that
  // was there and we delete `q`
  Page* p = nullptr;
  Page* q = new Page();
  if (pages_[page].compare_exchange_strong(p, q, RELEASE, RELAXED)) {
    return q;
  }
  else {
    delete q;
    return p;
  }
}

Bitmap::Word
Bitmap::addBit(const unsigned page, const unsigned offset)
{
  if (getPage(page)->addBit(offset) && page) {
    return addBit(page - 1, Words * sizeof(Word));
  }
  else {
    return pages_[0].load(ACQUIRE)->baseWord();
  }
}

bool
Bitmap::Page::addBit(const unsigned word, const unsigned bit)
{
  unsigned i = word;
  Word mask = Word(1) << bit;
  while (words_[i].fetch_add(mask, ACQ_REL) + mask > mask) {
    if (i-- == 0) {
      return true;
    }
    mask = Word(1);
  }
  return false;
}
