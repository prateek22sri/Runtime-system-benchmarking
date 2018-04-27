#ifndef PHOTON_TEST_COMMON
#define PHOTON_TEST_COMMON

#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <err.h>

#if defined(linux)
#define HAVE_SETAFFINITY
#include <sched.h>
#endif

#ifndef CPU_SETSIZE
#undef HAVE_SETAFFINITY
#endif

// don't allow affinity by default
#undef HAVE_SETAFFINITY

void CHECK(int e);
void static dbg_wait(void);
void setup_rand_seed();
int pick_rand_rank(int r, int n);

// threads
void *photon_wait_any_thread();
void *alloc_thread();

#define NALLOC         8192
#define ALLOC_SIZE     (1024*1024*2)

extern int DONE;

#endif
