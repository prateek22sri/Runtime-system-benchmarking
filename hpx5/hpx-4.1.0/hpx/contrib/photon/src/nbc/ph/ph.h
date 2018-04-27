#ifndef __PH_H__
#define __PH_H__

#include <photon_backend.h>
#include <photon_buffer.h>
#include <photon_exchange.h>
#include <photon_pwc.h>
#include <stdbool.h>
#include <stdio.h>
#include "list.h"

extern struct photon_config_t nbc_cfg;

extern bool __initialized ;

#define PH_OK true

typedef struct photon_nbc_req_t {
  bool send_req;
  int sink;
  int myrank;
  char *req_buf;
  int buf_size;
  int rselect; // recv buffer select
  int status;
} photon_nbc_req;

// allocate base buffer length
#define BASE_BUF_LENGTH 1024 * 64 // 64K

// encode first 4 bits as tags other 60 bits for handle ids
#define PHOTON_LOCAL_TAG_1 0x2 // 3232112
#define PHOTON_LOCAL_TAG_2 0x3 // 4423115
#define PHOTON_REMOTE_TAG_1 0xa // 0xcafebabe
#define PHOTON_REMOTE_TAG_2 0xf // 0xdeadbeef

#define NUM_BUFFERS_PER_DEST 50 // define how many buffers in the pipeline
//#define NUM_BUFFERS_PER_DEST 1 // define how many buffers in the pipeline
#define MAX_PROGRESS_ROUNDS 10 // define how many buffers in the pipeline
                               //  for each remote put op (per remote buffer)
#define NUM_PROCS_ULIMIT 200

#define NBC_PHOTON_REQ_COMPLETED 1
#define NBC_PHOTON_REQ_NOT_READY 2
#define NBC_PHOTON_REQ_ERROR 4
#define NBC_PHOTON_REQ_PENDING 0

typedef struct eager_buf {
  void *data;
  int size;
} eager_buf_t;

typedef struct recv_info {
  List *ordered_recvs; // temp array of recieves for this handle
  eager_buf_t *eager_recvs; // eager buffers
} recv_info_t;

typedef struct bufpool {
  char **send; // send buffer list - use a seperate buffer for each destination
  char **recv; // recv buffer list
  photon_rid *recvReq;
  photon_rid *sendReq;
  struct photon_buffer_t *rbuf_list; // remote buffers for pwc
  int *credits; // credits for flow control , for each destination remote buffer
  int *rselect; // current recv buffer `slot`
  int *sselect; // send buffer select
  two_lock_queue_t *credits_sources; // source ranks for each credit recieve
  int credits_released;
  // queue temp_pwc_reqs;
  void (*cleanup)(void *, int);
} buffer_pool_t;

typedef enum {
  RBUF_FETCH_OK = 1001,
  RBUF_FETCH_ERROR,
  RBUF_FETCH_ERROR_ZERO_CREDIT
} photon_rc_t;

typedef struct {
  photon_rc_t rc;
  struct photon_buffer_t *rbuf;
} get_remote_ret_t;

typedef struct completion_info {
  photon_cid req;
  int flag;
  int src;
  bool local;
  void *eager_data;
  int eager_size;
  bool done;
} completion_info_t;

get_remote_ret_t get_remote_photon_buffer(int dest, int buffer_size,
                                          buffer_pool_t *bp);
int wait_local(int pending);

int PHOTON_remote_put_wc(photon_nbc_req *, buffer_pool_t *bp, uint64_t tag);
bool PHOTON_Testall(int req_count, photon_nbc_req *req_array, int *flag,
                    buffer_pool_t *bp);
bool PHOTON_Testall_PWC(int req_count, photon_nbc_req *req_array, int *flag,
                        buffer_pool_t *bp, recv_info_t *recvs,
                        completion_info_t **inf, int inf_size, uint64_t tag);
void PHOTON_Check_credits(buffer_pool_t *bp, completion_info_t **comp,
                          int comp_size, int tag);
int init_photon_backend(int my_address, int total_nodes);
int init_photon_buffers(int nproc, buffer_pool_t *);
int register_photon_buffers(void *buffer_pool, int pool_sz,
                            int *buffer_lengths);
#endif
