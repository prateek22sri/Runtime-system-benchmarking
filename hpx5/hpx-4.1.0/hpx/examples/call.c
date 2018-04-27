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

#include <stdio.h>
#include <hpx/hpx.h>

static int _my_interrupt_handler(void) {
  printf("Hi, I am an interrupt!\n");
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_INTERRUPT, 0, _my_interrupt, _my_interrupt_handler);

static int _my_task_handler(void) {
  printf("Hi, I am a task!\n");
  return hpx_call_cc(HPX_HERE, _my_interrupt);
}
static HPX_ACTION(HPX_TASK, 0, _my_task, _my_task_handler);

static int _my_action_handler(void) {
  printf("Hi, I am an action!\n");
  return hpx_call_cc(HPX_HERE, _my_task);
}
static HPX_ACTION(HPX_DEFAULT, 0, _my_action, _my_action_handler);

static int _my_typed_handler(int i, float f, char c) {
  printf("Hi, I am a typed action with args: %d %f %c!\n", i, f, c);
  return hpx_call_cc(HPX_HERE, _my_action);
}
static HPX_ACTION(HPX_DEFAULT, 0, _my_typed, _my_typed_handler, HPX_INT,
                  HPX_FLOAT, HPX_CHAR);


static int _cont_fut_handler(hpx_addr_t future) {
  printf("Continuing an LCO to a typed continuation action.\n");
  return hpx_thread_continue(&future);
}
static HPX_ACTION(HPX_DEFAULT, 0, _cont_fut, _cont_fut_handler, HPX_ADDR);

static int _typed_cont_handler(hpx_addr_t future) {
  printf("Hi, I am a typed continuation action.\n");
  hpx_lco_set(future, 0, NULL, HPX_NULL, HPX_NULL);
  return HPX_SUCCESS;
}
static HPX_ACTION(HPX_DEFAULT, 0, _typed_cont, _typed_cont_handler, HPX_ADDR);

static int _main_handler(void) {
  int i = 42;
  float f = 1.0;
  char c = 'a';

  hpx_call_sync(HPX_HERE, _my_typed, NULL, 0, &i, &f, &c);
  hpx_addr_t done = hpx_lco_future_new(0);
  hpx_call_with_continuation(HPX_HERE, _cont_fut, HPX_HERE, _typed_cont, &done);
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);
  hpx_exit(0, NULL);
}
static HPX_ACTION(HPX_DEFAULT, 0, _main, _main_handler);

int main(int argc, char *argv[]) {
  hpx_init(&argc, &argv);
  int e = hpx_run(&_main, NULL);
  hpx_finalize();
  return e;
}
