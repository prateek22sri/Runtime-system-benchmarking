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

/// @file libhpx/util/options.c
/// @brief Implements libhpx configuration parsing and handling.
///

#include "parser.h"
#include "libhpx/action.h"
#include "libhpx/config.h"
#include "libhpx/debug.h"
#include "libhpx/libhpx.h"
#include "libhpx/locality.h"
#include "libhpx/parcel.h"
#include "libhpx/system.h"
#include "libhpx/util/Env.h"
#include "hpx/hpx.h"
#include <cassert>
#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <sstream>

typedef struct hpx_options_t hpx_options_t;

/// Special case handling for a config file option.
static char *_config_file = NULL;

/// The default configuration.
static const config_t _default_cfg = {
#define LIBHPX_OPT(group, id, init, UNUSED2) .group##id = init,
# include "libhpx/options.def"
#undef LIBHPX_OPT
};

/// Get a configuration value from an environment variable.
///
/// This function looks up a configuration value for an environment variable @p
/// var. If a value is not found it will look for an uppercase version. If a
/// value was found, we append a command-line-option form of the variable to @p
/// str so that it can be later parsed by our gengetopt infrastructure.
///
/// We need two different versions of the key we're looking for, one with
/// underscores to pass to getopt, and one with hyphens to pass to gengetopt.
///
/// @param[out]     str A dynamic string to store the output into.
/// @param          var The key we are looking for in the environment.
/// @param          opt The key we will use for gengetopt.
/// @param         flag Indicates if the option key is a flag or not.
static void _from_env(std::stringstream& ss, const char * const var,
                      const char * const arg, bool flag) {
  try {
    std::string s = libhpx::util::getEnv<std::string>(var);
    if (flag) {
      ss << "--" << arg << " ";
    } else {
      ss << "--" << arg << "=" << s << " ";
    }
  }
  catch (const libhpx::util::NotFound&) { }
}

/// Merge the option id and the group.
///
/// We need the key for the environment and the key for gengetopt as separate
/// things in the -from_env call. The function takes the env key, copies it, and
/// replaces underscores with hyphens.
static void _xform_string(std::stringstream& str, const char *env, bool flag) {
  char *opt = strdup(env);
  dbg_assert(opt);
  for (int i = 0, e = strlen(env); i < e; ++i) {
    if (opt[i] == '_') {
      opt[i] = '-';
    }
  }
  _from_env(str, env, opt, flag);
  free(opt);
}

/// Get values from the environment for the options declared in options.def.
///
/// This uses multiple-inclusion of options.def to call _get_env() for all of
/// the options declared there. Each found option will be appended to @p str in
/// a form that can be parsed by the gengetopt infrastructure.
///
/// @param[out]     str This will contain the collected values.
static void _from_env_all(std::stringstream& str) {
#define LIBHPX_OPT_FLAG(g, id, u3) _xform_string(str, "hpx_"#g#id, true);
#define LIBHPX_OPT(g, id, u3, u4) _xform_string(str, "hpx_"#g#id, false);
# include "libhpx/options.def"
#undef LIBHPX_OPT_FLAG
#undef LIBHPX_OPT
}

/// Accumulate configuration options from the environment.
///
/// This will go through the environment and create a command-line-like string
/// that we can parse using the gengetopt command-line infrastructure.
///
/// @param[out]    opts The option structure we will fill from the environment.
static void _process_env(hpx_options_t *opts) {
  std::stringstream hpx_opts;
  _from_env_all(hpx_opts);

  const std::string s = hpx_opts.str();
  const char *cmdline = s.c_str();
  if (cmdline) {
    const char *progname = system_get_program_name();
    int e = hpx_option_parser_string(cmdline, opts, progname);
    dbg_check(e, "failed to parse environment options: %s.\n", cmdline);
  }
}

/// Accumulate configuration options from the command line.
///
/// This will go through the command line, splitting out options that are
/// prefixed with "--hpx-" and processing them into the hpx_options_t.
///
/// @param[out]    opts The option structure we will fill from the environment.
/// @param         argc The number of arguments to process.
/// @param         argv The arguments to process.
static void _process_cmdline(hpx_options_t *opts, int *argc, char ***argv) {
  // Split the arguments into those that should be parsed as --hpx- options
  // and those that are application level options
  std::string hpx_opts, tmp;
  for (int i = 0, n = 0, e = *argc; i < e; ++i) {
    tmp += (*argv)[i];
    tmp += " ";
    std::size_t found = tmp.find("--hpx-");
    if (found == std::string::npos) {
      (*argv)[n++] = (*argv)[i];
    }
    else {
      hpx_opts += tmp;
      *argc = *argc - 1;
    }
    tmp.clear();
  }

  const char *cmdline = hpx_opts.c_str();
  if (cmdline) {
    int e = hpx_option_parser_string(cmdline, opts, argv[0][0]);
    dbg_check(e, "failed to parse command-line options %s.\n", cmdline);
  }
}

/// Process a bitvector opt.
///
/// @param            n The number of args we saw.
/// @param         args The args from gengetopt.
/// @param          all An arg that we should interpret as meaning "all bits".
///
/// @returns            A bitvector set with the bits specified in @p args.
static uint64_t _merge_bitvector(int n, uint32_t args[], uint64_t all) {
  uint64_t bits = 0;
  for (int i = 0; i < n; ++i) {
    if (args[i] == all) {
      return LIBHPX_OPT_BITSET_ALL;
    }
    bits |= 1 << args[i];
  }
  return bits;
}

/// Process a vector opt.
///
/// @param            n The number of option args we need to process.
/// @param         args The option args to process.
/// @param         init A default value.
/// @param         term A value that means "no more values".
///
/// @returns            A @p term-terminated vector populated with the @p
///                       args. This vector must be deleted at shutdown.
static int *_merge_vector(int n, int args[], int init, int term) {
  int *vector = new int[n + 1];
  for (int i = 0; i < n; ++i) {
    vector[i] = args[i];
  }
  vector[n] = (n) ? term : init;
  return vector;
}

/// Transform a set of options from gengetopt into a config structure.
///
/// This will only overwrite parts of the config structure for which options
/// were actually given.
///
/// @param          cfg The configuration object we are writing to.
/// @param         opts The gengetopt options we are reading from.
static void _merge_opts(config_t *cfg, const hpx_options_t *opts) {

#define LIBHPX_OPT_FLAG(group, id, UNUSED2)         \
  if (opts->hpx_##group##id##_given) {              \
    cfg->group##id = opts->hpx_##group##id##_flag;  \
  }

#define LIBHPX_OPT_SCALAR(group, id, UNUSED2, type)     \
  if (opts->hpx_##group##id##_given) {                  \
    cfg->group##id = (type)opts->hpx_##group##id##_arg; \
  }

#define LIBHPX_OPT_STRING(group, id, init)                  \
  if (opts->hpx_##group##id##_given) {                      \
    cfg->group##id = strdup(opts->hpx_##group##id##_arg);   \
  }

#define LIBHPX_OPT_BITSET(group, id, init)                                    \
  if (opts->hpx_##group##id##_given) {                                        \
    cfg->group##id = _merge_bitvector(opts->hpx_##group##id##_given,          \
                                      (uint32_t*)opts->hpx_##group##id##_arg, \
                                      hpx_##group##id##_arg_all);             \
  }

#define LIBHPX_OPT_INTSET(group, id, init, none, all)               \
  if (opts->hpx_##group##id##_given) {                              \
    cfg->group##id = _merge_vector(opts->hpx_##group##id##_given,   \
                                   opts->hpx_##group##id##_arg,     \
                                   init, none);                     \
  }
# include "libhpx/options.def"
#undef LIBHPX_OPT_INTSET
#undef LIBHPX_OPT_BITSET
#undef LIBHPX_OPT_STRING
#undef LIBHPX_OPT_SCALAR
#undef LIBHPX_OPT_STRING
#undef LIBHPX_OPT_FLAG

  if (opts->hpx_help_given) {
    hpx_print_version();
    libhpx_print_version();
    hpx_print_help();
    exit(EXIT_SUCCESS);
  }

  if (opts->hpx_version_given) {
    hpx_print_version();
    libhpx_print_version();
  }

  // Special case handling for the config file option, the
  // opts->hpx_configfile_arg is deleted so we have to duplicate it.
  if (opts->hpx_configfile_given) {
    dbg_assert(opts->hpx_configfile_arg);
    if (_config_file) {
      free(_config_file);
    }
    _config_file = strdup(opts->hpx_configfile_arg);
  }
}

/// Print the help associated with the HPX runtime options
void hpx_print_help(void) {
  hpx_option_parser_print_help();
}

#define LIBHPX_OPT_INTSET(group, id, init, none, all)                \
  int config_##group##id##_isset(const config_t *cfg, int element) { \
    if (!cfg->group##id) {                                           \
      return (init == all);                                          \
    }                                                                \
    for (int i = 0; cfg->group##id[i] != none; ++i) {                \
      if (cfg->group##id[i] == all) {                                \
        return 1;                                                    \
      }                                                              \
      if (cfg->group##id[i] == element) {                            \
        return 1;                                                    \
      }                                                              \
    }                                                                \
    return 0;                                                        \
  }
# include "libhpx/options.def"
#undef LIBHPX_OPT_INTSET

/// Parse HPX command-line options to create a new config object.
config_t *config_new(int *argc, char ***argv) {

  // first, initialize to the default configuration
  config_t *cfg = new config_t();
  dbg_assert(cfg);
  *cfg = _default_cfg;

  // Process the environment.
  hpx_options_t opts;
  _process_env(&opts);
  _merge_opts(cfg, &opts);
  hpx_option_parser_free(&opts);

  // Use the command line arguments to override the environment values.
  if (argc) {
    _process_cmdline(&opts, argc, argv);
    _merge_opts(cfg, &opts);
  }

  hpx_option_parser_free(&opts);

  // the config file takes the highest precedence in determining the
  // runtime parameters
  if (_config_file) {
    struct hpx_option_parser_params *params = hpx_option_parser_params_create();
    params->initialize = 0;
    params->override = 0;
    int e = hpx_option_parser_config_file(_config_file, &opts, params);
    dbg_check(e, "could not parse HPX configuration file: %s\n", _config_file);
    _merge_opts(cfg, &opts);

    free(_config_file);
    free(params);
    hpx_option_parser_free(&opts);
  }

#ifdef __GLIBC__
  optind = 0;
#else
  optind = 1;
#endif

  return cfg;
}

void config_print(const config_t *cfg, FILE *f) {
  fprintf(f, "------------------------\n"
             "HPX PARSED CONFIGURATION\n"
             "------------------------\n");
  fprintf(f, "General\n");
  fprintf(f, "  heapsize\t\t%zu\n", cfg->heapsize);
  fprintf(f, "  gas\t\t\t\"%s\"\n", HPX_GAS_TO_STRING[cfg->gas]);
  fprintf(f, "  boot\t\t\t\"%s\"\n", HPX_BOOT_TO_STRING[cfg->boot]);
  fprintf(f, "  transport\t\t\"%s\"\n", HPX_TRANSPORT_TO_STRING[cfg->transport]);
  fprintf(f, "  network\t\t\"%s\"\n", HPX_NETWORK_TO_STRING[cfg->network]);

  fprintf(f, "\nScheduler\n");
  fprintf(f, "  threads\t\t%d\n", cfg->threads);
  fprintf(f, "  stacksize\t\t%u\n", cfg->stacksize);
  fprintf(f, "  wfthreshold\t\t%u\n", cfg->sched_wfthreshold);
  fprintf(f, "  stackcachelimit\t%u\n", cfg->sched_stackcachelimit);

  fprintf(f, "\nLogging\n");
  fprintf(f, "  level\t\t\t");
  for (int i = 0, e = _HPX_NELEM(HPX_LOG_LEVEL_TO_STRING); i < e; ++i) {
    uint64_t level = 1lu << i;
    if (cfg->log_level & level) {
      fprintf(f, "\"%s\", ", HPX_LOG_LEVEL_TO_STRING[i]);
    }
  }
  fprintf(f, "\n");
  fprintf(f, "  at\t\t\t");
  if (!cfg->log_at) {
    fprintf(f, "all");
  }
  else {
    for (int i = 0; cfg->log_at[i] != HPX_LOCALITY_NONE; ++i) {
      if (cfg->log_at[i] == HPX_LOCALITY_ALL) {
        fprintf(f, "all\n");
        break;
      }
      else {
        fprintf(f, "%d, ", cfg->log_at[i]);
      }
    }
  }
  fprintf(f, "\n");

  fprintf(f, "\nDebugging\n");
  fprintf(f, "  mprotectstacks\t%d\n", cfg->dbg_mprotectstacks);
  fprintf(f, "  waitonabort\t\t%d\n", cfg->dbg_waitonabort);
  fprintf(f, "  waitonsig\t\t");
  for (int i = 0, e = _HPX_NELEM(HPX_WAITON_TO_STRING); i < e; ++i) {
    uint64_t signal = 1lu << i;
    if (cfg->dbg_waitonsig & signal) {
      fprintf(f, "\"%s\", ", HPX_WAITON_TO_STRING[i]);
    }
  }
  fprintf(f, "\n");
  fprintf(f, "  waitat\t\t");
  if (!cfg->dbg_waitat) {
    fprintf(f, "all");
  }
  else {
    for (int i = 0; cfg->dbg_waitat[i] != HPX_LOCALITY_NONE; ++i) {
      if (cfg->dbg_waitat[i] == HPX_LOCALITY_ALL) {
        fprintf(f, "all\n");
        break;
      }
      else {
        fprintf(f, "%d, ", cfg->dbg_waitat[i]);
      }
    }
  }
  fprintf(f, "\n");

  fprintf(f, "\nTracinf\n");
  fprintf(f, "  dir\t\t\t\"%s\"\n", cfg->trace_dir);
  fprintf(f, "  trace buffer size\t%zu\n", cfg->trace_buffersize);
  fprintf(f, "  trace classes\t\t");
  for (int i = 0, e = _HPX_NELEM(HPX_TRACE_CLASS_TO_STRING); i < e; ++i) {
    uint64_t type = (1lu << i);
    if (cfg->trace_classes & type) {
      fprintf(f, "\"%s\", ", HPX_TRACE_CLASS_TO_STRING[i]);
    }
  }
  fprintf(f, "\n");
  fprintf(f, "  at\t\t\t");
  if (!cfg->trace_at) {
    fprintf(f, "all");
  }
  else {
    for (int i = 0; cfg->trace_at[i] != HPX_LOCALITY_NONE; ++i) {
      if (cfg->trace_at[i] == HPX_LOCALITY_ALL) {
        fprintf(f, "all\n");
        break;
      }
      else {
        fprintf(f, "%d, ", cfg->trace_at[i]);
      }
    }
  }
  fprintf(f, "\n");

#ifdef HAVE_PHOTON
  fprintf(f, "\nPWC\n");
  fprintf(f, "  parcelbuffersize\t%lu\n", cfg->pwc_parcelbuffersize);
  fprintf(f, "  parceleagerlimit\t%lu\n", cfg->pwc_parceleagerlimit);
#endif

#ifdef HAVE_MPI
  fprintf(f, "\nIsend/Irecv\n");
  fprintf(f, "  testwindow\t\t%u\n", cfg->isir_testwindow);
  fprintf(f, "  sendlimit\t\t%u\n", cfg->isir_sendlimit);
  fprintf(f, "  recvlimit\t\t%u\n", cfg->isir_recvlimit);
#endif

#ifdef HAVE_PHOTON
  fprintf(f, "\nPhoton\n");
  fprintf(f, "  backend\t\t%s\n",
      PHOTON_BACKEND_TO_STRING[cfg->photon_backend]);
  fprintf(f, "  ibport\t\t%d\n", cfg->photon_ibport);
  fprintf(f, "  ledgersize\t\t%d\n", cfg->photon_ledgersize);
  fprintf(f, "  eagerbufsize\t\t%d\n", cfg->photon_eagerbufsize);
  fprintf(f, "  smallpwcsize\t\t%d\n", cfg->photon_smallpwcsize);
  fprintf(f, "  maxrd\t\t\t%d\n", cfg->photon_maxrd);
  fprintf(f, "  defaultrd\t\t%d\n", cfg->photon_defaultrd);
  fprintf(f, "  numcq\t\t\t%d\n", cfg->photon_numcq);
  fprintf(f, "  usecma\t\t%d\n", cfg->photon_usecma);
  fprintf(f, "  ethdev\t\t\"%s\"\n", cfg->photon_ethdev);
  fprintf(f, "  ibdev\t\t\t\"%s\"\n", cfg->photon_ibdev);
#endif
  fprintf(f, "\nOptimization\n");
  fprintf(f, "  smp\t\t\t%d\n", cfg->opt_smp);

  fprintf(f, "\nCoalescing parameters\n");
  fprintf(f, " Coalescing buffer size\t\t%d\n", cfg->coalescing_buffersize);


  fprintf(f, "------------------------\n");
}

void config_delete(config_t *cfg) {
  if (!cfg) {
    return;
  }

  if (cfg->log_at) {
    free(cfg->log_at);
  }

  if (cfg->dbg_waitat) {
    free(cfg->dbg_waitat);
  }

  delete cfg;
}
