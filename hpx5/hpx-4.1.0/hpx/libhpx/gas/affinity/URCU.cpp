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

#include "libhpx/gas/Affinity.h"
#include "libhpx/debug.h"
#include <city_hasher.hh>
#include <urcu-qsbr.h>
#include <urcu/rculfhash.h>
#include <stdexcept>
#include <vector>

namespace {
using libhpx::gas::Affinity;
using libhpx::gas::affinity::URCU;

/// Map to the correct CityHash handler.
/// @{
template <size_t N = sizeof(unsigned long)>
unsigned long Hash(hpx_addr_t gva);

template <> HPX_USED
unsigned long Hash<8>(hpx_addr_t gva) {
  return CityHash64(reinterpret_cast<const char* const>(&gva), sizeof(gva));
}

template <> HPX_USED
unsigned long Hash<4>(hpx_addr_t gva) {
  return CityHash32(reinterpret_cast<const char* const>(&gva), sizeof(gva));
}
/// @}

struct RCUReadScope {
  RCUReadScope() {
    rcu_read_lock();
  }

  ~RCUReadScope() {
    rcu_read_unlock();
  }
};
}

/// A hash table node, inherits from the RCU hash table node type.
struct URCU::Node : public cds_lfht_node {
  Node(hpx_addr_t k, int v) : key(k), value(v) {
    cds_lfht_node_init(this);
  }

  ~Node() {
  }

  static int Match(struct cds_lfht_node *node, const void *key) {
    const Node& n = *static_cast<const Node*>(node);
    const hpx_addr_t& k = *static_cast<const hpx_addr_t*>(key);
    return (n.key == k);
  }

  hpx_addr_t key;
  hpx_addr_t value;
};

URCU::URCU()
    : Affinity(),
      ht(cds_lfht_new(1, 1, 0, CDS_LFHT_AUTO_RESIZE, NULL))
{
}

URCU::~URCU()
{
  std::vector<Node*> nodes;
  removeAll(nodes);
  synchronize_rcu();

  for (int i = 0, e = nodes.size(); i < e; ++i) {
    delete nodes[i];
  }

  if (cds_lfht_destroy(ht, NULL) != 0) {
    dbg_error("Failed to destroy the urcu hash table\n");
  }
}

void
URCU::setAffinity(hpx_addr_t k, int v)
{
  if (Node *n = insert(Hash(k), new Node(k, v))) {
    synchronize_rcu();
    delete n;
  }
}

int
URCU::getAffinity(hpx_addr_t k) const
{
  return lookup(Hash(k), k);
}

void
URCU::clearAffinity(hpx_addr_t k)
{
  if (Node *n = remove(Hash(k), k)) {
    synchronize_rcu();
    delete n;
  }
}

URCU::Node*
URCU::insert(hash_t hash, URCU::Node *node)
{
  RCUReadScope _;
  hpx_addr_t key = node->key;
  void *n = cds_lfht_add_replace(ht, hash, Node::Match, &key, node);
  return static_cast<Node*>(n);
}

URCU::Node*
URCU::remove(hash_t hash, hpx_addr_t key)
{
  RCUReadScope _;
  struct cds_lfht_iter it;
  cds_lfht_lookup(ht, hash, Node::Match, &key, &it);
  if (Node *n = static_cast<Node*>(cds_lfht_iter_get_node(&it))) {
    if (cds_lfht_del(ht, n) == 0) {
      return n;
    }
  }
  return NULL;
}

void
URCU::removeAll(std::vector<URCU::Node*>& nodes)
{
  RCUReadScope _;
  struct cds_lfht_iter it;
  struct cds_lfht_node *node;
  cds_lfht_for_each(ht, &it, node) {
    if (cds_lfht_del(ht, node) != 0) {
      throw std::runtime_error("Failed to delete node\n");
    }
    nodes.push_back(static_cast<Node*>(node));
  }
}

int
URCU::lookup(hash_t hash, hpx_addr_t key) const
{
  RCUReadScope _;
  struct cds_lfht_iter it;
  cds_lfht_lookup(ht, hash, Node::Match, &key, &it);
  Node *n = static_cast<Node*>(cds_lfht_iter_get_node(&it));
  return (n) ? n->value : -1;
}
