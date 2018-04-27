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

#include "Trace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pwd.h>

#include <libhpx/debug.h>
#include <libhpx/libhpx.h>
#include <libhpx/Scheduler.h>
#include "libhpx/Worker.h"
#include <hpx/hpx.h>
#include "metadata.h"
#include "file.h"

namespace {
using libhpx::Worker;
}

#ifndef HOST_NAME_MAX
# define HOST_NAME_MAX 255
#endif

/// The logtable structure.
///
/// This is used to represent all of the data needed to keep the state
/// of an individual event log
typedef struct logtable {
  int               fd;               //!< file backing the log
  int         event_id;               //!< the event we're logging
  int     record_bytes;               //!< record size
  char         *buffer;               //!< pointer to buffer
  char * volatile next;               //!< pointer to next record
  size_t      max_size;               //!< max size in bytes
  size_t   header_size;               //!< header size in bytes
} logtable_t;

/// The path to the log directory.
///
/// This path is set up during inst_init either based on the --hpx-inst-dir
/// runtime option, or using a temporary directory if no dir is specified. It is
/// not initialized if we are not logging instrumentation at this locality, so
/// the special value of NULL is meaningful.
///
/// @{
static const char *_log_path;
/// @}

/// Concatenate two paths.
///
/// This will return a dynamically allocated, '\0' terminated string that joins
/// the two paths with a '/'. The client must free the returned string to
/// prevent memory leaks.
///
/// @param          lhs The left-hand-side of the path.
/// @param          rhs The right-hand-side of the path.
///
/// @return             A dynamically allocated string that is the concatenated
///                     path: "lhs + / + rhs", must be freed by the caller.
static char *_concat_path(const char *lhs, const char *rhs) {
  // length is +1 for '/' and +1 for \00
  int bytes = strlen(lhs) + strlen(rhs) + 2;
  char *out = static_cast<char*>(malloc(bytes));
  snprintf(out, bytes, "%s/%s", lhs, rhs);
  return out;
}

/// Open an output log file of the given name in the log directory.
///
/// This will fopen a file relative to the _log_path, as long as the _log_path
/// is non-NULL. If the _log_path is NULL this is interpreted as disabling
/// instrumentation at this rank and we just return NULL. If the fopen fails
/// this will log an error and return NULL.
///
/// @param          name The name of the file to open.
/// @param             e An error string to print if the fopen fails.
///
/// @return              The opened file, or NULL if no file was opened.
static HPX_NON_NULL(1, 2) FILE *_fopen_log(const char *name, const char *e) {
  if (!_log_path) {
    return NULL;
  }

  char *path = _concat_path(_log_path, name);
  FILE *file = fopen(path, "w");
  if (!file) {
    log_error("failed to open file %s\n %s\n", path, e);
  }
  free(path);
  return file;
}

static const char *_mkdir(const char *dir) {
  // try and create the directory---we don't care if it's already there
  if (mkdir(dir, 0777) && errno != EEXIST) {
    log_error("Could not create %s for instrumentation\n", dir);
  }
  return dir;
}

static const char *_mktmp(void) {
  // create directory name
  time_t t = time(NULL);
  struct tm lt;
  localtime_r(&t, &lt);
  char dirname[256];
  struct passwd *pwd = getpwuid(getuid());
  const char *username = pwd->pw_name;
  snprintf(dirname, 256, "hpx-%s.%.4d%.2d%.2d.%.2d%.2d", username,
           lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min);
  const char *path = _concat_path("/tmp", dirname);
  log_internal(__LINE__, __FILE__, __func__,
               "logging instrumentation files into %s\n", path);
  return _mkdir(path);
}

static const char *_get_log_path(const char *dir) {
  if (!dir) {
    return _mktmp();
  }

  struct stat sb;
  // if the path doesn't exist, then we make it
  if (stat(dir, &sb) != 0) {
    return _mkdir(dir);
  }

  // if the path is a directory, we just return it
  if (S_ISDIR(sb.st_mode)) {
    return strndup(dir, 256);
  }

  // path exists but isn't a directory.
  log_error("--hpx-trace-dir=%s does not point to a directory\n", dir);
  return NULL;
}

static int _create_file(const char *filename, size_t size) {
  static const int flags = O_RDWR | O_CREAT;
  static const int perm = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
  int fd = open(filename, flags, perm);
  if (fd == -1) {
    log_error("failed to open a log file for %s\n", filename);
    return -1;
  }

  return fd;
}

/// Dump a file that will map this locality ID to a host hame.
static void _dump_hostnames(void) {
  char hostname[HOST_NAME_MAX];
  gethostname(hostname, HOST_NAME_MAX);

  char filename[256];
  snprintf(filename, 256, "hostname.%d", HPX_LOCALITY_ID);

  FILE *file = _fopen_log(filename, "failed to open hostname file");
  if (!file) {
    return;
  }

  fprintf(file, "%s\n", hostname);
  fclose(file);
}

/// This will output a list of action ids and names as a two-column csv file
/// This is so that traced parcels can be interpreted more easily.
static void _dump_actions(void) {
  char filename[256];
  snprintf(filename, 256, "actions.%d.csv", hpx_get_my_rank());

  FILE *file = _fopen_log(filename, "failed to open action file");
  if (!file) {
    return;
  }

  for (int i = 1, e = action_table_size(); i < e; ++i) {
    const char *tag = action_is_internal(i) ? "INTERNAL" : "USER";
    fprintf(file, "%d,%s,%s\n", i, actions[i].key, tag);
  }
  fclose(file);
}

/// Initialize a logtable.
///
/// This will create a file-backed mmaped buffer for this event for logging
/// purposes.
///
/// @param           lt The log table to initialize.
/// @param     filename The name of the file to create and map.
/// @param         size The number of bytes to allocate.
/// @param         type The event class.
/// @param     event_id The event type.
/// @param    worker_id ID of the current worker
static void logtable_init(logtable_t *log, const char* filename, size_t size,
                   int type, int event_id, int worker_id) {
  log->fd = -1;
  log->event_id = event_id;
  log->record_bytes = 0;
  log->max_size = size;

  if (filename == NULL || size == 0) {
    return;
  }

  log->fd = _create_file(filename, size);
  if (log->fd == -1) {
    log_error("could not create log file %s\n", filename);
    return;
  }

  log->buffer = static_cast<char*>(malloc(size));
  if (!log->buffer) {
    log_error("problem allocating buffer for %s\n", filename);
    close(log->fd);
    return;
  }

  int fields = TRACE_EVENT_NUM_FIELDS[event_id];
  log->record_bytes = sizeof(record_t) + fields * sizeof(uint64_t);
  log->next = log->buffer - log->record_bytes;

  char *buffer = static_cast<char*>(calloc(1, 32768));
  log->header_size = write_trace_header(buffer, type, event_id, worker_id);
  if (write(log->fd, buffer, log->header_size) != (ssize_t)log->header_size) {
    log_error("failed to write header to file\n");
  }
  free(buffer);
}

/// Clean up a log table.
///
/// It is safe to call this on an uninitialized log table. If the log table was
/// initialized this will unmap its buffer, and truncate and close the
/// corresponding file.
///
/// @param           lt The log table to finalize.
static void logtable_fini(logtable_t *log) {
  if (!log || log->fd <= 0 || !log->max_size) {
    return;
  }

  size_t filesize = log->next - log->buffer;
  write(log->fd, log->buffer, filesize);
  if (close(log->fd)) {
    log_error("failed to close trace file\n");
  }
}

static
void _create_logtable(int worker_id, int type, int event_id, size_t size) {
  char filename[256];
  snprintf(filename, 256, "%05d.%03d.event.%03d.%s.log",
           hpx_get_my_rank(), worker_id, event_id,
           TRACE_EVENT_TO_STRING[event_id]);

  char *path = _concat_path(_log_path, filename);
  logtable_t *log = &here->logs[worker_id][event_id];
  logtable_init(log, path, size, type, event_id, worker_id);
  free(path);
}

namespace {
using libhpx::self;
using libhpx::instrumentation::Trace;
class FileTracer : public Trace {
 public:
  FileTracer(const config_t *cfg) : Trace(cfg) {
  }

  ~FileTracer() {
  }

  // int type() const {
  //   return HPX_TRACE_BACKEND_FILE;
  // }

  /// Append a record to a log table.
  ///
  /// Log tables have variable length record sizes based on their event type. This
  /// allows the user to log a variable number of features with each trace point.
  ///
  /// @param       UNUSED Unused argument.
  /// @param            n The number of features to log.
  /// @param           id The event id.
  /// @param         args The features to log.
  void vappend(int UNUSED, int n, int event_id, va_list& vargs) {
    const int worker_id = self->getId();
    logtable_t *log = &here->logs[worker_id][event_id];
    dbg_assert(log);
    uint64_t time = hpx_time_from_start_ns(hpx_time_now());
    char *next = log->next + log->record_bytes;
    dbg_assert(next >= log->buffer);
    if ((size_t)(next - log->buffer) > log->max_size) {
      EVENT_TRACE_FILE_IO_BEGIN();
      write(log->fd, log->buffer, log->next - log->buffer);
      EVENT_TRACE_FILE_IO_END();
      next = log->buffer;
    }
    log->next = next;

    record_t *r = (record_t*)next;
    r->ns = time;
    for (int i = 0, e = n; i < e; ++i) {
      r->user[i] = va_arg(vargs, uint64_t);
    }
  }

  void start(void) {
    // Allocate memory for pointers to the logs
    const unsigned nworkers = here->sched->getNWorkers();
    here->logs = new logtable_t*[nworkers];
    for (unsigned w = 0; w < nworkers; ++w) {
      const size_t size = sizeof(logtable_t) * TRACE_NUM_EVENTS;
      if (posix_memalign((void**)&here->logs[w], HPX_CACHELINE_SIZE, size)) {
        dbg_error("could not allocate aligned buffer\n");
      }
      memset(here->logs[w], 0, size);
      // Scan through each trace event and create logs for the associated
      // events that that we are going to be tracing.
      for (unsigned i = 0; i < TRACE_NUM_EVENTS; ++i) {
        int c = TRACE_EVENT_TO_CLASS[i];
        if (inst_trace_class(c)) {
          _create_logtable(w, c, i, here->config->trace_buffersize);
        }
      }
    }

    // If we're tracing parcels then we need to output the actions and the
    // hostnames as well. The "_dump" actions won't do anything if we're not
    // instrumenting locally.
    // @todo 1. Shouldn't we dump these for all ranks, even if we're not
    //          instrumenting *at* that rank? Otherwise we'll be missing data for
    //          ranks that *are* instrumenting.
    //       2. Why can't we just do this during initialization?
    if (inst_trace_class(HPX_TRACE_PARCEL)) {
      _dump_actions();
      _dump_hostnames();
    }
  }

  void destroy(void) {
    if (_log_path) {
      free((char*)_log_path);
      _log_path = NULL;
    }

    if (here->logs) {
      for (int w = 0; w < here->sched->getNWorkers(); ++w) {
        // deallocate the log tables
        for (unsigned i = 0, e = TRACE_NUM_EVENTS; i < e; ++i) {
          logtable_fini(&here->logs[w][i]);
        }
        free(here->logs[w]);
        here->logs[w] = nullptr;
      }
      delete[] here->logs;
      here->logs = nullptr;
    }
  }
};
}

void *trace_file_new(const config_t *cfg) {
  // At this point we know that we'll be generating some sort of logs, so
  // prepare the path.
  _log_path = _get_log_path(cfg->trace_dir);
  if (!_log_path) {
    return NULL;
  }

  return new FileTracer(cfg);
}
