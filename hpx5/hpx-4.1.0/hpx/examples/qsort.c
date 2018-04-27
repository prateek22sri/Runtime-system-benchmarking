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

/*
-Joshua Stough
-Washington and Lee University
-Quicksort a random list of size given by the argument (default 1M)
-Time both sequential quicksort and parallel.
-$ ./quicksort [1000000]
*/
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "hpx/hpx.h"

#define DNUM 1000000
#define THREAD_LEVEL 10

static void _usage(FILE *f, int error) {
  fprintf(f, "Usage: fibonacci [options] NUMBER\n"
          "\t-h, show help\n");
  hpx_print_help();
  fflush(f);
  exit(error);
}

//for sequential and parallel implementation
void swap(double lyst[], int i, int j);
int partition(double lyst[], int lo, int hi);
void quicksortHelper(double lyst[], int lo, int hi);
void quicksort(double lyst[], int size);
int isSorted(double lyst[], int size);

static hpx_action_t _main = 0;
static hpx_action_t _parallelQuicksortHelper = 0;
//for parallel implementation
int parallelQuicksort(double lyst[], int size, int tlevel);
static int _parallelQuicksortHelper_action(void *threadarg, size_t size);
struct thread_data{
    double *lyst;
    int low;
    int high;
    int level;
};
//thread_data should be thread-safe, since while lyst is
//shared, [low, high] will not overlap among threads.

//for the builtin libc qsort:
int compare_doubles (const void *a, const void *b);

/*
Main action:
-generate random list
-time sequential quicksort
-time parallel quicksort
-time standard qsort
*/
static int _main_action(uint64_t *args, size_t size) {
  hpx_time_t start;
  srand(time(NULL)); //seed random
  uint64_t NUM = *args;

  printf("parallel Quick Sort for %"PRIu64"\n", NUM);
  //Want to compare sorting on the same list,
  //so backup.
  double *lystbck = (double *) malloc(NUM*sizeof(double));
  double *lyst = (double *) malloc(NUM*sizeof(double));

  //Populate random original/backup list.
  for (int i = 0; i < NUM; i ++) {
    lystbck[i] = 1.0*rand()/RAND_MAX;
  }
  //copy list.
  memcpy(lyst, lystbck, NUM*sizeof(double));

  //Sequential mergesort, and timing
  start = hpx_time_now();
  quicksort(lyst, NUM);
  printf("Sequential quicksort took: %g ms. \n", hpx_time_elapsed_ms(start));
  if (!isSorted(lyst, NUM)) {
    printf("Oops, lyst did not get sorted by quicksort.\n");
  }

  //Now, parallel quicksort.
  //copy list.
  memcpy(lyst, lystbck, NUM*sizeof(double));
  start = hpx_time_now();
  parallelQuicksort(lyst, NUM, THREAD_LEVEL);
  printf("Parallel quicksort took: %g ms.\n", hpx_time_elapsed_ms(start));
  if (!isSorted(lyst, NUM)) {
    printf("Oops, lyst did not get sorted by parallelQuicksort.\n");
  }

  //Finally, built-in for reference:
  memcpy(lyst, lystbck, NUM*sizeof(double));
  start = hpx_time_now();
  qsort(lyst, NUM, sizeof(double), compare_doubles);
  //Compute time difference.
  printf("Built-in qsort took: %g ms.\n\n", hpx_time_elapsed_ms(start));
  if (!isSorted(lyst, NUM)) {
    printf("Oops, lyst did not get sorted by qsort.\n");
  }

  free(lyst);
  free(lystbck);
  hpx_exit(0, NULL);
}

int main (int argc, char *argv[])
{
  int opt = 0;
  srand(time(NULL)); //seed random
  uint64_t NUM = DNUM;

  // Register the main action
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _main, _main_action,
                      HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED, _parallelQuicksortHelper,
                      _parallelQuicksortHelper_action, HPX_POINTER, HPX_SIZE_T);

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  while ((opt = getopt(argc, argv,  "h?")) != -1) {
    switch (opt) {
     case 'h':
      _usage(stdout, EXIT_SUCCESS);
     case '?':
     default:
      _usage(stderr, EXIT_FAILURE);
    }
  }

  argc -= optind;
  argv += optind;

  switch (argc) {
   case 0:
     fprintf(stderr, "\nMissing user specified list size.\n"); // fall through
   default:
     _usage(stderr, EXIT_FAILURE);
   case 1:
     NUM = atoi(argv[0]);
     break;
  }

  // Run the main action
  e = hpx_run(&_main, NULL, &NUM, sizeof(NUM));
  hpx_finalize();
  return e;
}

void quicksort(double lyst[], int size)
{
  quicksortHelper(lyst, 0, size-1);
}

void quicksortHelper(double lyst[], int lo, int hi)
{
  if (lo >= hi) return;
  int b = partition(lyst, lo, hi);
  quicksortHelper(lyst, lo, b-1);
  quicksortHelper(lyst, b+1, hi);
}
void swap(double lyst[], int i, int j)
{
  double temp = lyst[i];
  lyst[i] = lyst[j];
  lyst[j] = temp;
}

int partition(double *lyst, int lo, int hi)
{
  int pivot = (int) ((lo + hi)/2);
  //int pivot = (int) (lo + (hi-lo + 1)*(1.0*rand()/RAND_MAX));
  double pivotValue = lyst[pivot];
  swap(lyst, pivot, hi);
  int storeIndex = lo;
  for (int i=lo ; i<hi ; i++) {
    if (lyst[i] <= pivotValue) {
      swap(lyst, i, storeIndex);
      storeIndex++;
    }
  }
  swap(lyst, storeIndex, hi);
  return storeIndex;
}

/*
parallel quicksort top level:
instantiate parallelQuicksortHelper thread, and that's
basically it.
*/
int parallelQuicksort(double lyst[], int size, int tlevel)
{
  hpx_addr_t theThread = HPX_HERE;
  struct thread_data td;
  td.lyst = lyst;
  td.low = 0;
  td.high = size - 1;
  td.level = tlevel;

  //The top-level thread
  hpx_addr_t done = hpx_lco_future_new(sizeof(uint64_t));
  hpx_call(theThread, _parallelQuicksortHelper, done, &td, sizeof(td));
  hpx_lco_wait(done);
  hpx_lco_delete(done, HPX_NULL);

  return HPX_SUCCESS;
}

/*
parallelQuicksortHelper
-if the level is still > 0, then partition and make
parallelQuicksortHelper threads to solve the left and
right-hand sides, then quit. Otherwise, call sequential.
*/
static int _parallelQuicksortHelper_action(void *threadarg, size_t size)
{
  int mid, t;

  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;

  //fyi:
  //printf("Thread responsible for [%d, %d], level %d.\n",
        //my_data->low, my_data->high, my_data->level);

  if (my_data->level <= 0 || my_data->low == my_data->high) {
    //We have plenty of threads, finish with sequential.
    quicksortHelper(my_data->lyst, my_data->low, my_data->high);
    return HPX_SUCCESS;
  }

  //Now we partition our part of the lyst.
  mid = partition(my_data->lyst, my_data->low, my_data->high);

  //At this point, we will create threads for the
  //left and right sides.  Must create their data args.
  struct thread_data thread_data_array[2];

  for (t = 0; t < 2; t ++) {
    thread_data_array[t].lyst = my_data->lyst;
    thread_data_array[t].level = my_data->level - 1;
  }
  thread_data_array[0].low = my_data->low;
  if (mid > my_data->low) {
    thread_data_array[0].high = mid-1;
  } else {
    thread_data_array[0].high = my_data->low;
  }
  if (mid < my_data->high) {
    thread_data_array[1].low = mid+1;
  } else {
    thread_data_array[1].low = my_data->high;
  }
  thread_data_array[1].high = my_data->high;

  //Now, instantiate the threads.
  //In quicksort of course, due to the transitive property,
  //no elements in the left and right sides of mid will have
  //to be compared again.
  hpx_addr_t futures[] = {
    hpx_lco_future_new(sizeof(int)),
    hpx_lco_future_new(sizeof(int))
  };

  hpx_addr_t threads[] = {
    HPX_HERE,
    HPX_HERE
  };

  int pqs[] = {
    0,
    0
  };

  void *addrs[] = {
    &pqs[0],
    &pqs[1]
  };

  size_t sizes[] = {
    sizeof(int),
    sizeof(int)
  };

  for (t = 0; t < 2; t++){
    hpx_call(threads[t], _parallelQuicksortHelper, futures[t],
             (void *)&thread_data_array[t], sizeof(thread_data_array[t]));
  }
  hpx_lco_get_all(2, futures, sizes, addrs, NULL);
  hpx_lco_delete(futures[0], HPX_NULL);
  hpx_lco_delete(futures[1], HPX_NULL);
  return HPX_SUCCESS;
}

//check if the elements of lyst are in non-decreasing order.
//one is success.
int isSorted(double lyst[], int size)
{
  for (int i = 1; i < size; i ++) {
    if (lyst[i] < lyst[i-1]) {
      printf("at loc %d, %e < %e \n", i, lyst[i], lyst[i-1]);
      return 0;
    }
  }
  return 1;
}

//for the built-in qsort comparator
//from http://www.gnu.org/software/libc/manual/html_node/Comparison-Functions.html#Comparison-Functions
int compare_doubles (const void *a, const void *b)
{
  const double *da = (const double *) a;
  const double *db = (const double *) b;

  return (*da > *db) - (*da < *db);
}
