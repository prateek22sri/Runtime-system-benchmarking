// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#ifndef PQUEUE_WRAPPER_H
#define PQUEUE_WRAPPER_H

#include <stdio.h>
#include<pthread.h>
#include "pqueue.h"


typedef struct {
  pqueue_t *pq;
  pthread_mutex_t mutex; //mutex associated with the priority queue
  // tatas_lock_t mutex;
} locked_pq;

typedef locked_pq sssp_queue_t;

#include <stdlib.h>

typedef struct {
  hpx_addr_t vertex;
  distance_t distance;
  size_t pos; //needed by the pqueue interface
} pqueue_node;

/* priority queue functions needed for priority init function */

static int cmp_pri(distance_t next, distance_t curr) {
  return (next < curr);
}

static distance_t get_pri(void *a) {
  return ((pqueue_node *) a)->distance;
}

static void set_pri(void *a, distance_t pri) {
  ((pqueue_node *) a)->distance = pri;
}

static size_t get_pos(void *a) {
  return ((pqueue_node *) a)->pos;
}

static void set_pos(void *a, size_t pos) {
  ((pqueue_node *) a)->pos = pos;
}

static size_t sssp_queue_pop(sssp_queue_t *q, hpx_addr_t *v, distance_t *d) {
  // printf("sssp_queue_pop on queue %" PRIxPTR "\n", (uintptr_t)q);
  pqueue_node *head;
  // printf("Inside the pop function\n");
  pthread_mutex_lock(&(q->mutex));
  //sync_tatas_acquire(&(q->mutex)); // acquire the mutex
  const size_t size = pqueue_size(q->pq);
  if (size == 0) {
    pthread_mutex_unlock(&(q->mutex));
    // sync_tatas_release(&(q->mutex));
    return 0;
  }
  head = pqueue_pop(q->pq);
  // printf("Popped one vertex\n");
  pthread_mutex_unlock(&(q->mutex));
  //sync_tatas_release(&(q->mutex)); // release it
  assert(head != NULL);
  *v = head->vertex;
  *d = head->distance;
  free(head);
  return size;
}

static bool sssp_queue_push(sssp_queue_t *q, hpx_addr_t v, distance_t d) {
  // printf("sssp_queue_push on queue %" PRIxPTR "\n", (uintptr_t)q);
  int success;
  pqueue_node *pq_node = (pqueue_node*) malloc(sizeof(pqueue_node));
  pq_node->vertex = v;
  pq_node->distance = d;
  pthread_mutex_lock(&(q->mutex));
  // sync_tatas_acquire(&(q->mutex)); // acquire the mutex
  bool empty = pqueue_size(q->pq) == 0;
  success = pqueue_insert(q->pq, pq_node);
  pthread_mutex_unlock(&(q->mutex));
  // sync_tatas_release(&(q->mutex)); // release it
  assert(success == 0);
  // printf("sssp_queue_push finished on queue %" PRIxPTR "\n", (uintptr_t)q);
  return empty;
}

static sssp_queue_t *sssp_queue_create(size_t num_elem) {
  sssp_queue_t *q = malloc(sizeof(locked_pq));
  q->pq = pqueue_init(num_elem, cmp_pri, get_pri, set_pri, get_pos, set_pos);
  // printf("returned from initializing the priority queue\n");
  //TODO: don't need to initialize the mutex?
  //pq_array[i]->mutex = hpx_lco_sema_new(1);
  pthread_mutex_init(&q->mutex, NULL);
  //sync_tatas_init(&q->mutex);
  // printf("sssp_queue_create finished on queue %" PRIxPTR "\n", (uintptr_t)q);
  return q;
}

static void sssp_queue_free(sssp_queue_t *q) {
  pqueue_free(q->pq);
  free(q);
}

/* static size_t sssp_queue_size(sssp_queue_t *q) { */
/*   // printf("sssp_queue_size on queue %" PRIxPTR "\n", (uintptr_t)q); */
/*   sync_tatas_acquire(&(q->mutex)); */
/*   const size_t size = pqueue_size(q->pq); */
/*   sync_tatas_release(&(q->mutex)); */
/*   return size; */
/* } */

#endif // PQUEUE_WRAPPER_H
