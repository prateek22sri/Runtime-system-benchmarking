// =============================================================================
//  High Performance ParalleX Library (libapex)
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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "libapex/apex_global_hpx.h"

//#define APEX_LOCALITY ((HPX_LOCALITIES)-1) // doesn't seem to work...
#define APEX_LOCALITY 0
apex_profile value;
apex_profile reduced_value;
int min_rank = 0;
int max_rank = 0;
int max_threads = 0;
int* thread_caps = NULL;
double* previous_values = NULL;
double* moving_average = NULL;
double* current_values = NULL;
const int window_size = 20;

hpx_action_t apex_get_value = 0;
hpx_action_t apex_set_local_cap = 0;
hpx_action_t apex_allreduce = 0;
apex_function_address profiled_action = 0L;
char profiled_action_name[256];
apex_profiler_type profiler_type;
bool good_values = false;

#define APEX_WARMUP_ITERS 10

FILE *graph_output;

bool apex_set_new_thread_caps(int count, apex_profile values[count]) {
  static int countdown = 10; // wait for the application to warm up
  if (countdown > 0) {
    countdown = countdown - 1;
    return false;
  }
  // divide max by max_threads - or the current thread cap for the max rank
  //double period_max = (reduced_value.maximum - previous_values[max_rank]);
  double period_max = reduced_value.maximum;
  // set each node thread cap to a relative multiple of that
  for (int i = 0; i < count; ++i) {
    int old_cap = thread_caps[i];
    // how much accumulated in the last period?
    //double last_period = values[i].accumulated - previous_values[i];
    double last_period = values[i].accumulated;
    double ratio = last_period / period_max;
    // if this is the max rank, increase the thread count, limited to max.
    if (i == max_rank) {
      //thread_caps[i] = fmin(max_threads, (thread_caps[i]+1));
      thread_caps[i] = max_threads;
    } else {
      // if I did significantly less work than max, reduce my thread count
      int new_cap = fmin(max_threads, ceil(ratio * max_threads));
      if (new_cap != thread_caps[i]) {
        thread_caps[i] = fmax(apex_get_throttling_min_threads(), new_cap);
      }
    }
    printf("Locality %d: %f (%f) of new work from %d threads, new cap: %d\n", i, last_period, ratio, old_cap, thread_caps[i]);
  }
  return true;
}

bool apex_set_new_thread_caps_hysteresis(int count, apex_profile values[count]) {
  static int countdown = APEX_WARMUP_ITERS; // wait for the application to warm up
  if (countdown > 0) {
    countdown = countdown - 1;
    for (int i = 0; i < count; ++i) {
      moving_average[i] = current_values[i] / (APEX_WARMUP_ITERS - countdown);
    }
    return false;
  }
  // divide max by max_threads - or the current thread cap for the max rank
  double last_period = (current_values[max_rank] - previous_values[max_rank]);
  double moving_max = ((moving_average[max_rank] * (window_size-1)) + last_period) / window_size;
  // set each node thread cap to a relative multiple of that
  for (int i = 0; i < count; ++i) {
    int old_cap = thread_caps[i];
    // how much accumulated in the last period?
    last_period = current_values[i] - previous_values[i];
    moving_average[i] = ((moving_average[i] * (window_size-1)) + last_period) / window_size;
    //double ratio = (moving_max - moving_average[i]) / moving_max;
    double ratio = moving_average[i] / moving_max;
    // if this is the max rank, increase the thread count, limited to max.
    if (i == max_rank) {
      //thread_caps[i] = fmin(max_threads, (thread_caps[i]+1));
      thread_caps[i] = max_threads;
    } else {
      // if I did significantly less work than max, reduce my thread count
      //if (ratio > 1.0/thread_caps[max_rank]) {
      int new_cap = fmin(max_threads, ceil(ratio * max_threads));
      if (new_cap != thread_caps[i]) {
        thread_caps[i] = fmax(apex_get_throttling_min_threads(), new_cap);
      }
    }
    printf("Locality %d: last=%f avg=%f (%f) of work from %d threads, new cap: %d\n", i, last_period, moving_average[i], ratio, old_cap, thread_caps[i]);
  }
  return true;
}

void apex_sum(int count, apex_profile values[count]) {
  // swap the current and previous values, for relative differences
  double* tmp = current_values;
  current_values = previous_values;
  previous_values = tmp;
  reduced_value.calls = values[0].calls;
  reduced_value.accumulated = values[0].accumulated;
  reduced_value.sum_squares = values[0].sum_squares;
  //reduced_value.minimum = values[0].minimum;
  //reduced_value.maximum = values[0].maximum;
  reduced_value.minimum = values[0].accumulated;
  reduced_value.maximum = values[0].accumulated;
  current_values[0] = values[0].accumulated;
  min_rank = max_rank = 0;
  int local_good_values = (values[0].calls > 0.0 && values[0].accumulated > 0.0) ? 1 : 0;
  for (int i = 1; i < count; ++i) {
      reduced_value.calls += values[i].calls;
      reduced_value.accumulated += values[i].accumulated;
      reduced_value.sum_squares += values[i].sum_squares;
      //reduced_value.minimum = fmin(reduced_value.minimum,values[i].minimum);
      double accum = values[i].accumulated;
      if (accum < reduced_value.minimum) {
        reduced_value.minimum = accum;
        min_rank = i;
      }
      //reduced_value.maximum = fmax(reduced_value.maximum,values[i].maximum);
      if (accum > reduced_value.maximum) {
        reduced_value.maximum = accum;
        max_rank = i;
      }
      current_values[i] = values[i].accumulated;
      local_good_values = local_good_values + ((values[i].calls > 0) ? 1 : 0);
  }
  printf("good values: %d of %d\n", local_good_values, count);
  if (local_good_values == count) good_values = true;
  return ;
}

int apex_get_value_action(void *args) {
  apex_profile * p;
  if (profiler_type == APEX_FUNCTION_ADDRESS) {
    p = apex_get_profile(APEX_FUNCTION_ADDRESS, (void*)profiled_action);
  } else {
    p = apex_get_profile(APEX_NAME_STRING, (void*)profiled_action_name);
  }
  if (p != NULL) {
    value.calls = p->calls;
    value.accumulated = p->accumulated;
    value.sum_squares = p->sum_squares;
    value.minimum = p->minimum;
    value.maximum = p->maximum;
  }
  return HPX_THREAD_CONTINUE(value);
}

int apex_set_local_cap_action(void *args, size_t size) {
  int local_cap = *(int*)args;
  printf("At rank %d received value %d\n", hpx_get_my_rank(), local_cap);
  apex_set_thread_cap(local_cap);
  return HPX_SUCCESS;
}

int apex_allreduce_action(void *unused) {
  int num_ranks = HPX_LOCALITIES;
  int my_rank = HPX_LOCALITY_ID;
  // The APEX sink is the only one to make the requests
  if (my_rank != APEX_LOCALITY) {
      return HPX_SUCCESS;
  }

  apex_profile values[num_ranks];
  void         *addrs[num_ranks];
  size_t        sizes[num_ranks];
  hpx_addr_t   futures[num_ranks];

  for (int i = 0; i < num_ranks; ++i) {
    addrs[i] = &values[i];
    sizes[i] = sizeof(apex_profile);
    futures[i] = hpx_lco_future_new(sizeof(apex_profile));
    hpx_call(HPX_THERE(i), apex_get_value, futures[i], NULL, 0);
  }

  hpx_lco_get_all(num_ranks, futures, sizes, addrs, NULL);
  apex_sum(num_ranks, values);
  bool ready = apex_set_new_thread_caps(num_ranks, values);

  if (ready && good_values) {
    // set new caps for everyone
    for (int i = 0; i < num_ranks; ++i) {
        hpx_lco_delete(futures[i], HPX_NULL);
        futures[i] = hpx_lco_future_new(0);
        hpx_call(HPX_THERE(i), apex_set_local_cap, futures[i], &(thread_caps[i]), sizeof(thread_caps[i]));
    }

    hpx_lco_get_all(num_ranks, futures, sizes, addrs, NULL);
  }

  for (int i = 0; i < num_ranks; ++i) {
    hpx_lco_delete(futures[i], HPX_NULL);
  }
  return HPX_SUCCESS;
}

void apex_trigger_reduction(void) {
  int my_rank = HPX_LOCALITY_ID;
  if (my_rank == APEX_LOCALITY) {
    hpx_addr_t future = hpx_lco_future_new(sizeof(int));
    // fire and forget...
    hpx_call(HPX_HERE, apex_allreduce, future, NULL, 0);
    //hpx_call(APEX_LOCALITY, apex_allreduce, future, NULL, 0);
    //apex_allreduce_action(NULL);
  }
  return;
}

int apex_periodic_output(apex_context const context) {
  int my_rank = HPX_LOCALITY_ID;
  if (my_rank != APEX_LOCALITY) return APEX_NOERROR;
  double avg = 0.0;
  double stddev = 0.0;
  if (reduced_value.calls > 0.0) {
    avg = reduced_value.accumulated / reduced_value.calls;
    stddev = sqrt((reduced_value.sum_squares / reduced_value.calls) - (avg*avg));
  }
  printf("Iterations: calls=%lu mean=%f +/- %f, min=%f (%d), max=%f (%d)\r", (unsigned long)reduced_value.calls, avg, stddev, reduced_value.minimum, min_rank, reduced_value.maximum, max_rank);
  fflush(stdout);
  fprintf(graph_output,"%lu\t%f\t%f\t%f\t%d\t%f\t%d\n",(unsigned long)reduced_value.calls, avg, stddev, reduced_value.minimum, min_rank, reduced_value.maximum, max_rank);
  fflush(graph_output);
  return APEX_NOERROR;
}

void apex_global_setup(apex_profiler_type type, void* in_action) {
  profiler_type = type;
  if (type == APEX_FUNCTION_ADDRESS) {
    profiled_action = (apex_function_address)in_action;
  } else {
    strcpy(profiled_action_name, in_action);
  }
  apex_register_periodic_policy(1000000, apex_periodic_output);
  //apex_set_use_policy(true);
  //set_value      = HPX_REGISTER_ACTION(action_apex_set_value);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
          apex_get_value, apex_get_value_action,
          HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
          apex_set_local_cap, apex_set_local_cap_action,
          HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
          apex_allreduce, apex_allreduce_action,
          HPX_POINTER, HPX_SIZE_T );
  graph_output = fopen("./profile_data.txt", "w");
  fprintf(graph_output,"\"calls\"\t\"mean\"\t\"stddev\"\t\"min\"\t\"min_rank\"\t\"max\"\t\"max_rank\"\n");
  fflush(graph_output);
  // get the max number of threads. All throttling will be done relative to this.
  max_threads = hpx_get_num_threads();
  thread_caps = calloc(HPX_LOCALITIES, sizeof(int));
  for (int i = 0; i < HPX_LOCALITIES; ++i) {
    thread_caps[i] = max_threads;
  }
  previous_values = calloc(HPX_LOCALITIES, sizeof(double));
  moving_average = calloc(HPX_LOCALITIES, sizeof(double));
  current_values = calloc(HPX_LOCALITIES, sizeof(double));
}

