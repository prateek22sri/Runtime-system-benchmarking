#include "common.h"
#include "photon.h"

int DONE = 0;

void CHECK(int e) {
  if (e != PHOTON_OK) {
    fprintf(stderr, "Photon operation not OK: %d\n", e);
    exit(1);
  }
}

PHOTON_NO_OPTIMIZE void static dbg_wait(void) {
  int i = 0;
  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  printf("PID %d on %s ready for attach\n", getpid(), hostname);
  fflush(stdout);
  while (0 == i)
    sleep(12);
}

void setup_rand_seed() {
  int rdata;
  int ur = open("/dev/urandom", O_RDONLY);
  int rc = read(ur, &rdata, sizeof rdata);
  srand(rdata);
  close(ur);
}

int pick_rand_rank(int r, int n) {
  int j = rand() % n;
  // send to random rank, excluding self
  while (j == r)
    j = rand() % n;
  return j;
}

void *photon_wait_any_thread() {
  photon_rid request;
  int flag, rc;
  
  do {
    rc = photon_wait_any(&flag, &request);
    if (rc != PHOTON_OK) {
      fprintf(stderr, "Error in photon_wait_any\n");
      exit(1);
    }
  } while (!DONE);

  pthread_exit(NULL);
}

void *alloc_thread() {
  char *bufs[NALLOC];
  int i;
  for (i=0; i<NALLOC; i++)
    bufs[i] = NULL;
  
  i = 0;
  do {
    bufs[i] = calloc(1, ALLOC_SIZE);
    photon_register_buffer(bufs[i], ALLOC_SIZE);
    if (i >= NALLOC/2) {
      photon_unregister_buffer(bufs[i-NALLOC/2], ALLOC_SIZE);
      free(bufs[i-NALLOC/2]);
      bufs[i-NALLOC/2] = NULL;
    }
    else if (bufs[i+NALLOC/2] != NULL) {
      photon_unregister_buffer(bufs[i+NALLOC/2], ALLOC_SIZE);
      free(bufs[i+NALLOC/2]);
      bufs[i+NALLOC/2] = NULL;
    }
    i++;
    i = i % NALLOC;
    usleep(100);
  } while(!DONE);

  pthread_exit(NULL);
}
