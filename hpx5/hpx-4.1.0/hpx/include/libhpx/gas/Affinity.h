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

#ifndef LIBHPX_GAS_AFFINITY_H
#define LIBHPX_GAS_AFFINITY_H

#include "hpx/hpx.h"
#include <cuckoohash_map.hh>
#include <city_hasher.hh>

extern "C" struct cds_lfht;

namespace libhpx {
namespace gas {
class Affinity {
 public:
  virtual void setAffinity(hpx_addr_t gva, int worker) = 0;
  virtual void clearAffinity(hpx_addr_t gva) = 0;
  virtual int getAffinity(hpx_addr_t gva) const = 0;

 protected:
  virtual ~Affinity();
};

namespace affinity {
class None : public virtual libhpx::gas::Affinity {
 public:
  None();
  ~None();

  void setAffinity(hpx_addr_t gva, int worker) { }
  void clearAffinity(hpx_addr_t gva) { }
  int getAffinity(hpx_addr_t gva) const { return -1; }
};

class URCU : public virtual libhpx::gas::Affinity {
 public:
  URCU();
  ~URCU();

  void setAffinity(hpx_addr_t gva, int worker);
  void clearAffinity(hpx_addr_t gva);
  int getAffinity(hpx_addr_t gva) const;

 private:
  typedef unsigned long hash_t;
  struct Node;

  Node* insert(hash_t hash, Node* node);
  Node* remove(hash_t hash, hpx_addr_t key);
  void removeAll(std::vector<Node*>& nodes);
  int lookup(hash_t hash, hpx_addr_t key) const;

  struct cds_lfht * const ht;
};

class CuckooHash : public virtual libhpx::gas::Affinity {
 public:
  CuckooHash();
  ~CuckooHash();

  void setAffinity(hpx_addr_t gva, int worker);
  void clearAffinity(hpx_addr_t gva);
  int getAffinity(hpx_addr_t gva) const;

 private:
  cuckoohash_map<hpx_addr_t, int, CityHasher<hpx_addr_t> > map_;
};
}
}
}

#endif // LIBHPX_GAS_AFFINITY_H
