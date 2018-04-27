/*
 * Copyright 2011 Cray Inc.  All Rights Reserved.
 */

/*
 * This source file contains the common utility functions.
 */

#include <sched.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef CRAY_CONFIG_GHAL_ARIES
#include "aries/misc/exceptions.h"
#endif

#include "utility_functions.h"
#include "logging.h"

int             aborted = 0;
char           *command_name;
int             expected_passed = 0;
int             failed = 0;
int             passed = 0;

#define INCREMENT_ABORTED aborted++
#define INCREMENT_FAILED  failed++
#define INCREMENT_PASSED  passed++
#define MAXIMUM_CQ_RETRY_COUNT 500

#ifdef DEBUG
int v_option = 3;
#else
int v_option = 0;
#endif

#ifdef HAVE_PMI
#include <pmi.h>

/*
 * allgather gather the requested information from all of the ranks.
 */

void
allgather(void *in, void *out, int len) {
  static int      already_called = 0;
  int             i;
  static int     *ivec_ptr = NULL;
  static int      job_size = 0;
  int             my_rank;
  char           *out_ptr;
  int             rc;
  char           *tmp_buf;

  if (!already_called) {
    rc = PMI_Get_size(&job_size);
    assert(rc == PMI_SUCCESS);

    rc = PMI_Get_rank(&my_rank);
    assert(rc == PMI_SUCCESS);

    ivec_ptr = (int *) malloc(sizeof(int) * job_size);
    assert(ivec_ptr != NULL);

    rc = PMI_Allgather(&my_rank, ivec_ptr, sizeof(int));
    assert(rc == PMI_SUCCESS);

    already_called = 1;
  }

  tmp_buf = (char *) malloc(job_size * len);
  assert(tmp_buf);

  rc = PMI_Allgather(in, tmp_buf, len);
  assert(rc == PMI_SUCCESS);

  out_ptr = out;

  for (i = 0; i < job_size; i++) {
    memcpy(&out_ptr[len * ivec_ptr[i]], &tmp_buf[i * len], len);
  }

  free(tmp_buf);
}
#endif /* HAVE PMI */

/*
 * get_gni_nic_address get the nic address for the specified device.
 *
 *   Returns: the nic address for the specified device.
 */

unsigned int
get_gni_nic_address(int device_id) {
  int             alps_address = -1;
  int             alps_dev_id = -1;
  unsigned int    address,
           cpu_id;
  gni_return_t    status;
  int             i;
  char           *token,
                 *p_ptr;

  p_ptr = getenv("PMI_GNI_DEV_ID");
  if (!p_ptr) {

    /*
     * Get the nic address for the specified device.
     */

    status = GNI_CdmGetNicAddress(device_id, &address, &cpu_id);
    if (status != GNI_RC_SUCCESS) {
      dbg_err("GNI_CdmGetNicAddress ERROR status: %s (%d)", gni_err_str[status], status);
      abort();
    }
  }
  else {

    /*
     * Get the ALPS device id from the PMI_GNI_DEV_ID environment
     * variable.
     */

    while ((token = strtok(p_ptr, ":")) != NULL) {
      alps_dev_id = atoi(token);
      if (alps_dev_id == device_id) {
        break;
      }

      p_ptr = NULL;
    }

    assert(alps_dev_id != -1);

    p_ptr = getenv("PMI_GNI_LOC_ADDR");
    assert(p_ptr != NULL);

    i = 0;

    /*
     * Get the nic address for the ALPS device.
     */

    while ((token = strtok(p_ptr, ":")) != NULL) {
      if (i == alps_dev_id) {
        alps_address = atoi(token);
        break;
      }

      p_ptr = NULL;
      ++i;
    }

    assert(alps_address != -1);
    address = alps_address;
  }

  return address;
}

#ifdef HAVE_PMI
/*
 * gather_nic_addresses gather all of the nic addresses for all of the
 *                      other ranks.
 *
 *   Returns: an array of addresses for all of the nics from all of the
 *            other ranks.
 */

void    *
gather_nic_addresses(void) {
  size_t          addr_len;
  unsigned int   *all_addrs;
  unsigned int    local_addr;
  int             rc;
  int             size;

  /*
   * Get the size of the process group.
   */

  rc = PMI_Get_size(&size);
  assert(rc == PMI_SUCCESS);

  /*
   * Assuming a single gemini device.
   */

  local_addr = get_gni_nic_address(0);

  addr_len = sizeof(unsigned int);

  /*
   * Allocate a buffer to hold the nic address from all of the other
   * ranks.
   */

  all_addrs = (unsigned int *) malloc(addr_len * size);
  assert(all_addrs != NULL);

  /*
   * Get the nic addresses from all of the other ranks.
   */

  allgather(&local_addr, all_addrs, sizeof(int));

  return (void *) all_addrs;
}
#endif /* HAVE_PMI */

/*
 * get_cookie will get the cookie value associated with this process.
 *
 * Returns: the cookie value.
 */

uint32_t
get_cookie(void) {
  uint32_t        cookie;
  char           *p_ptr;
  char           *token;

  p_ptr = getenv("PMI_GNI_COOKIE");
  assert(p_ptr != NULL);

  token = strtok(p_ptr, ":");
  cookie = (uint32_t) atoi(token);

  return cookie;
}

/*
 * get_cq_event will process events from the completion queue.
 *
 *   cq_handle is the completion queue handle.
 *   uts_info contains the node name.
 *   rank_id is the rank of this process.
 *   source_cq determines if the CQ is a source or a
 *       destination completion queue.
 *   retry determines if GNI_CqGetEvent should be called multiple
 *       times or only once to get an event.
 *
 *   Returns:  gni_cq_entry_t for success
 *             0 on success
 *             1 on an error
 *             2 on an OVERRUN error
 *             3 on no event found error
 */

int
get_cq_event(gni_cq_handle_t cq_handle,
             unsigned int source_cq, unsigned int retry,
             gni_cq_entry_t *next_event) {
  gni_cq_entry_t  event_data = 0;
  uint64_t        event_type;
  gni_return_t    status = GNI_RC_SUCCESS;
  int             wait_count = 0;

  status = GNI_RC_NOT_DONE;
  while (status == GNI_RC_NOT_DONE) {

    /*
     * Get the next event from the specified completion queue handle.
     */

    status = GNI_CqGetEvent(cq_handle, &event_data);
    if (status == GNI_RC_SUCCESS) {
      *next_event = event_data;

      /*
       * Processed event succesfully.
       */

      if (v_option > 1) {
        event_type = GNI_CQ_GET_TYPE(event_data);

        if (event_type == GNI_CQ_EVENT_TYPE_POST) {
          if (source_cq == 1) {
            dbg_trace("GNI_CqGetEvent    source      type: POST(%lu) inst_id: %lu tid: %lu event: 0x%16.16lx",
                     event_type,
                     GNI_CQ_GET_INST_ID(event_data),
                     GNI_CQ_GET_TID(event_data),
                     event_data);
          }
          else {
            dbg_trace("GNI_CqGetEvent    destination type: POST(%lu) inst_id: %lu event: 0x%16.16lx",
                     event_type,
                     GNI_CQ_GET_INST_ID(event_data),
                     event_data);
          }
        }
        else if (event_type == GNI_CQ_EVENT_TYPE_SMSG) {
          if (source_cq == 1) {
            dbg_trace("GNI_CqGetEvent    source      type: SMSG(%lu) msg_id: 0x%8.8x event: 0x%16.16lx",
                     event_type,
                     (unsigned int) GNI_CQ_GET_MSG_ID(event_data),
                     event_data);
          }
          else {
            dbg_trace("GNI_CqGetEvent    destination type: SMSG(%lu) data: 0x%16.16lx event: 0x%16.16lx",
                     event_type,
                     GNI_CQ_GET_DATA(event_data),
                     event_data);
          }
        }
        else if (event_type == GNI_CQ_EVENT_TYPE_MSGQ) {
          if (source_cq == 1) {
            dbg_trace("GNI_CqGetEvent    source      type: MSGQ(%lu) msg_id: 0x%8.8x event: 0x%16.16lx",
                     event_type,
                     (unsigned int) GNI_CQ_GET_MSG_ID(event_data),
                     event_data);
          }
          else {
            dbg_trace("GNI_CqGetEvent    destination type: MSGQ(%lu) data: 0x%16.16lx event: 0x%16.16lx",
                     event_type,
                     GNI_CQ_GET_DATA(event_data),
                     event_data);
          }
        }
        else {
          if (source_cq == 1) {
            dbg_trace("GNI_CqGetEvent    source      type: %lu inst_id: %lu event: 0x%16.16lx",
                     event_type,
                     GNI_CQ_GET_DATA(event_data),
                     event_data);
          }
          else {
            dbg_trace("GNI_CqGetEvent    destination type: %lu data: 0x%16.16lx event: 0x%16.16lx",
                     event_type,
                     GNI_CQ_GET_DATA(event_data),
                     event_data);
          }
        }
      }

      return 0;
    }
    else if (status != GNI_RC_NOT_DONE) {
      int error_code = 1;

      /*
       * An error occurred getting the event.
       */

      char           *cqErrorStr;
      char           *cqOverrunErrorStr = "";
      gni_return_t    tmp_status = GNI_RC_SUCCESS;
#ifdef CRAY_CONFIG_GHAL_ARIES
      uint32_t        status_code;

      status_code = GNI_CQ_GET_STATUS(event_data);
      if (status_code == A_STATUS_AT_PROTECTION_ERR) {
        return 1;
      }
#endif

      /*
       * Did the event queue overrun condition occurred?
       * This means that all of the event queue entries were used up
       * and another event occurred, i.e. there was no entry available
       * to put the new event into.
       */

      if (GNI_CQ_OVERRUN(event_data)) {
        cqOverrunErrorStr = "CQ_OVERRUN detected ";
        error_code = 2;

        if (v_option > 2) {
          dbg_trace("ERROR CQ_OVERRUN detected");
        }
      }

      cqErrorStr = (char *) malloc(256);
      if (cqErrorStr != NULL) {

        /*
         * Print a user understandable error message.
         */

        tmp_status = GNI_CqErrorStr(event_data, cqErrorStr, 256);
        if (tmp_status == GNI_RC_SUCCESS) {
          dbg_err("GNI_CqGetEvent    ERROR %sstatus: %s (%d) inst_id: %lu event: 0x%16.16lx GNI_CqErrorStr: %s",
		  cqOverrunErrorStr, gni_err_str[status], status,
		  GNI_CQ_GET_INST_ID(event_data),
		  event_data,
		  cqErrorStr);
        }
        else {

          /*
           * Print the error number.
           */

          dbg_err("GNI_CqGetEvent    ERROR %sstatus: %s (%d) inst_id: %lu event: 0x%16.16lx",
		  cqOverrunErrorStr, gni_err_str[status], status,
		  GNI_CQ_GET_INST_ID(event_data),
		  event_data);
        }

        free(cqErrorStr);
      }
      else {

        /*
         * Print the error number.
         */

        dbg_err("GNI_CqGetEvent    ERROR %sstatus: %s (%d) inst_id: %lu event: 0x%16.16lx",
		cqOverrunErrorStr, gni_err_str[status], status,
		GNI_CQ_GET_INST_ID(event_data),
		event_data);
      }
      return error_code;
    }
    else if (retry == 0) {
      return 3;
    }
    else {

      /*
       * An event has not been received yet.
       */

      wait_count++;

      if (wait_count >= MAXIMUM_CQ_RETRY_COUNT) {
        /*
         * This prevents an indefinite retry, which could hang the
         * application.
         */

        dbg_trace("GNI_CqGetEvent    ERROR no event was received status: %d retry count: %d",
                 status, wait_count);
        return 3;
      }

      /*
       * Release the cpu to allow the event to be received.
       * This is basically a sleep, if other processes need to do some work.
       */

      if ((wait_count % (MAXIMUM_CQ_RETRY_COUNT / 10)) == 0) {
        /*
         * Sometimes it takes a little longer for
         * the datagram to arrive.
         */

        sleep(1);
      }
      else {
        sched_yield();
      }
    }
  }

  return 1;
}

/*
 * get_ptag will get the ptag value associated with this process.
 *
 * Returns: the ptag value.
 */

uint8_t
get_ptag(void) {
  char           *p_ptr;
  uint8_t         ptag;
  char           *token;

  p_ptr = getenv("PMI_GNI_PTAG");
  assert(p_ptr != NULL);

  token = strtok(p_ptr, ":");
  ptag = (uint8_t) atoi(token);

  return ptag;
}


#ifdef HAVE_PMI
/*
 * print_results will determine if the test was successful or not
 *               and then print a message according to this result.
 *
 *   Returns:  0 for a success
 *            -1 for a failure
 *            -2 for an abort
 */

inline int
print_results(void) {
  char            abort_string[256];
  char           *exit_status;
  int             rc;


  /*
   * Wait for the other ranks to get here.
   */

  rc = PMI_Barrier();
  assert(rc == PMI_SUCCESS);

  if (aborted > 0) {

    /*
     * This test aborted.
     */

    exit_status = "Aborted      ";
    rc = -2;
  }
  else if (failed > 0) {

    /*
     * This test failed.
     */

    exit_status = "Failed       ";
    rc = -1;
  }
  else if (passed != expected_passed) {

    /*
     * This test did not have the correct number of passes.
     */

    exit_status = "Indeterminate";
    rc = 0;
  }
  else {
    /*
     * This test executed successfully.
     */

    exit_status = "Passed       ";
    rc = 0;
  }

  /*
   * Print the results from this test.
   */

  dbg_trace("%s:    %s    Test Results    Passed: %i/%i Failed: %i Aborted: %i",
           command_name, exit_status,
           passed, expected_passed, failed, aborted);

  if (aborted > 0) {

    /*
     * Abort the application's other ranks.
     */

    sprintf(abort_string, "%s called abort", command_name);
    PMI_Abort(-1, abort_string);
  }

  return rc;
}
#endif /* HAVE_PMI */
