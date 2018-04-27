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

#include <unistd.h>
#include "hpx/hpx.h"
#include "tests.h"

static barrier_t *barrier = NULL;
static volatile int n = 0;
static char * volatile task_sp = NULL;

static int _test_task_handler(void) {
  char local;

  printf("thread %d running the subtask on stack %p\n", HPX_THREAD_ID, &local);
  fflush(stdout);

  // Record my stack address so that we can verify that an eager transfer really
  // did take place.
  task_sp = &local;

  // Let everyone else make progress---one of them should start running my
  // parent, if it's been exposed to the world. Otherwise, no one else will
  // run.
  sync_barrier_join(barrier, HPX_THREAD_ID);

  // Wait for a while. This give the rest of the threads plenty of time to steal
  // my parent if they're going to.
  sleep(1);

  // Bump the counter.
  sync_store(&n, 1, SYNC_RELEASE);

  // At this point my parent was stolen.
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_TASK, 0, _test_task, _test_task_handler);

static int _test_action_handler(void) {
  char local;

  // Everyone joins the barrier once.
  if (sync_barrier_join(barrier, HPX_THREAD_ID)) {
    // I win the race.
    printf("thread %d running action on stack %p\n", HPX_THREAD_ID, &local);

    // This will push the task onto my queue, then I have to induce myself to
    // transfer to it---everyone else is blocked, so all I have to do is call
    // yield, which should do the transfer on the same stack, and make this
    // thread available to whoever wakes up.
    //
    // Note that the _test_task task actually releases the lock here, this
    // prevents anyone from stealing the parent thread (or getting it from the
    // yield queue) until I have already transferred to the child.
    //
    // We send our continuation along so that the test doesn't terminate early.
    hpx_addr_t and = hpx_thread_current_cont_target();
    int e = hpx_call(HPX_HERE, _test_task, and);
    assert(e == HPX_SUCCESS);
    hpx_thread_yield();
    printf("action stolen by %d\n", HPX_THREAD_ID);

    // Now, this thread should have been "stolen" or taken from the yield queue
    // or whatnot. We expect that we're running concurrent with, and on the same
    // stack, as the _test_task. Verify that we're on the same stack.
    ptrdiff_t d = &local - task_sp;

    if (0 < d && d < 1000) {
      // We're on the same stack---for this to be safe, the _test_task MUST have
      // already run, which implies that the value for n must be 1.
      int v = sync_load(&n, SYNC_ACQUIRE);
      printf("stack difference is %td, value is %d\n", d, v);
      assert(v == 1 && "work-first task test failed\n");
    }
    else {
      printf("test indeterminate, task spawned with new stack, d=%td\n", d);
    }

    printf("work-first task test success\n");
  }
  else {
    // I lost the race, wait for the entire thing to be set up before
    // returning and becoming a "stealer".
    sync_barrier_join(barrier, HPX_THREAD_ID);
  }
  printf("finishing %d\n", HPX_THREAD_ID);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_action, _test_action_handler);

static int _test_try_task_handler(void) {
  barrier = sr_barrier_new(HPX_THREADS);
  assert(barrier);
  hpx_addr_t and = hpx_lco_and_new(HPX_THREADS + 1);
  assert(and);
  for (int i = 0; i < HPX_THREADS; ++i) {
    int e = hpx_call(HPX_HERE, _test_action, and);
    assert(e == HPX_SUCCESS);
  }
  hpx_lco_wait(and);
  hpx_lco_delete(and, HPX_NULL);
  sync_barrier_delete(barrier);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_try_task, _test_try_task_handler);

static HPX_ACTION_DECL(_test_recursion);
static int _test_recursion_handler(int n, hpx_addr_t and) {
  if (!--n) {
    return HPX_SUCCESS;
  }
  else {
    // not 'n' was decremented
    return hpx_xcall(HPX_HERE, _test_recursion, and, n, and);
  }
}
static HPX_ACTION(HPX_TASK, 0, _test_recursion,
                  _test_recursion_handler, HPX_INT, HPX_ADDR);

static int _test_recursion_top_handler(void) {
  static int DEPTH = 500;
  hpx_addr_t and = hpx_lco_and_new(DEPTH);
  int e = hpx_xcall(HPX_HERE, _test_recursion, and, DEPTH, and);
  if (HPX_SUCCESS == e) {
    e = hpx_lco_wait(and);
  }
  hpx_lco_delete(and, HPX_NULL);
  return e;
}
static HPX_ACTION(HPX_DEFAULT, 0, _test_recursion_top, _test_recursion_top_handler);

TEST_MAIN({
    ADD_TEST(_test_recursion_top, 0);
    ADD_TEST(_test_try_task, 0);
});
