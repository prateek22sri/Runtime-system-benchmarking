// ============================================================================
//  High Performance ParalleX Library (hpx-apps)
//
//  Copyright (c) 2013-2017, Trustees of Indiana University,
//  All rights reserved.
//
//  This software may be modified and distributed under the terms of the BSD
//  license.  See the COPYING file for details.
//
//  This software was created at the Indiana University Center for Research in
//  Extreme Scale Technologies (CREST).
// ============================================================================

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>
#include <stdatomic.h>

#include "hpx/hpx.h"
#include "pxgl/pxgl.h"

#include "../generatorhelper/include/prng.h"

const char *sssp_kind_str[] = {
  "Chaotic Relaxation",
  "Distributed Control",
  "New Distributed Control"
};

const char *graph_generator_type_str[] = {
  "Graph500 generator",
  "DIMACS generator"
};

static void _usage(FILE *stream) {
  fprintf(stream, "Usage: sssp [options] <graph-file> <problem-file>\n"
          "\t-a, instead resetting adj list between the runs, reallocate it\n"
      "\t-b, do not compute checksum (for speed up of correct tests)\n"
      "\t-c, use chaotic algorithm\n"
      "\t-d, use distributed control\n"
      "\t-e, run the KLA algorithm\n"
      "\t-f, frequency for distributed control\n"
      "\t-g, use groph500 generator with scale parameter\n"
          "\t-h, this help display\n"
          "\t-k, use and-lco-based termination detection\n"
      "\t-l, number of queues for distributed control (does not work with DC1)\n"
          "\t-p, use process-based termination detection\n"
          "\t-q, limit time for SSSP executions in seconds\n"
      "\t-s, nuber of problems to run\n"
      "\t-t, queue size threshold for chatoic relaxation\n"
      "\t-u, use the DC1 implementation of distributed control\n"
      "\t-y, yield count\n"
      "\t-z, use delta stepping (with chaotic or DC), specify delta parameter\n");
  hpx_print_help();
  fflush(stream);
}

static hpx_action_t _print_vertex_distance;
static int _print_vertex_distance_action(int *i)
{
  const hpx_addr_t target = hpx_thread_current_target();

  adj_list_vertex_t *vertex;
  if (!hpx_gas_try_pin(target, (void**)&vertex))
    return HPX_RESEND;

  printf("vertex: %d nbrs: %lu dist: %llu\n", *i, vertex->num_edges, vertex->distance);

  hpx_gas_unpin(target);
  return HPX_SUCCESS;
}


static hpx_action_t _print_vertex_distance_index;
static int _print_vertex_distance_index_action(int *i)
{
  const hpx_addr_t target = hpx_thread_current_target();

  hpx_addr_t *v;
  hpx_addr_t vertex;

  if (!hpx_gas_try_pin(target, (void**)&v))
    return HPX_RESEND;

  vertex = *v;
  hpx_gas_unpin(target);

  return hpx_call_sync(vertex, _print_vertex_distance, NULL, 0, i, sizeof(*i));
}

void sample_graph(sssp_uint_t **problems, sssp_uint_t nproblems, size_t num_edges, sssp_uint_t numvertices, adj_list_t *graph){
  *problems = malloc(nproblems * sizeof(sssp_uint_t));
  assert(*problems);
  // printf("calling sample root\n");
  sample_roots(*problems, nproblems, num_edges, numvertices, graph);
}

static int _read_dimacs_spec(char **filename, sssp_uint_t *nproblems, sssp_uint_t **problems) {
  FILE *f = fopen(*filename, "r");
  assert(f);

  char line[LINE_MAX];
  int count = 0;
  while (fgets(line, sizeof(line), f) != NULL) {
    switch (line[0]) {
      case 'c': continue;
      case 's':
        sscanf(&line[1], " %llu", &((*problems)[count++]));
        break;
      case 'p':
        sscanf(&line[1], " aux sp ss %" SSSP_UINT_PRI, nproblems);
        *problems = malloc(*nproblems * sizeof(sssp_uint_t));
        assert(*problems);
        break;
      default:
        fprintf(stderr, "invalid command specifier '%c' in problem file. skipping..\n", line[0]);
        continue;
    }
  }
  fclose(f);
  return 0;
}

// Arguments for the main SSSP action
typedef struct {
  char *filename;
  sssp_uint_t nproblems;
  sssp_uint_t *problems;
  char *prob_file;
  sssp_uint_t time_limit;
  int realloc_adj_list;
  sssp_kind_t sssp_kind;
  sssp_init_dc_args_t sssp_init_dc_args;
  size_t delta;
  termination_t termination;
  graph_generator_type_t graph_generator_type;
  int scale;
  int edgefactor;
  bool checksum;
  size_t level;
} _sssp_args_t;

static hpx_action_t _main;
static int _main_action(_sssp_args_t *args) {
  const int realloc_adj_list = args->realloc_adj_list;

  // set the termination-detection algorithm type.
  set_termination(args->termination);
  edge_list_t el;

  if (args->graph_generator_type == _GRAPH500) {
    // Create an edge list structure
    // printf("Generating edge-list as Graph500 spec\n");
    const graph500_edge_list_generator_args_t graph500_edge_list_generator_args = {
      .scale = args->scale,
      .locality_readers = HPX_LOCALITIES,
      .thread_readers = HPX_THREADS,
      .edgefactor = args->edgefactor
    };
    hpx_call_sync(HPX_HERE, graph500_edge_list_generator, &el, sizeof(el),
                  &graph500_edge_list_generator_args,
                  sizeof(graph500_edge_list_generator_args));
    printf("Edge List: #v = %llu, #e = %llu\n", el.num_vertices, el.num_edges);

  }

  else if (args->graph_generator_type == _DIMACS) {
    // Create an edge list structure from the given filename
    printf("Allocating edge-list from file %s.\n", args->filename);
    const edge_list_from_file_args_t edge_list_from_file_args = {
      .locality_readers = HPX_LOCALITIES,
      .thread_readers = HPX_THREADS/2 + 1,
      .filename = args->filename
    };
    hpx_call_sync(HPX_HERE, edge_list_from_file, &el, sizeof(el),
                  &edge_list_from_file_args, sizeof(edge_list_from_file_args));
    printf("Edge List: #v = %llu, #e = %llu\n",
       el.num_vertices, el.num_edges);
  }
  // Open the results file and write the basic info out
  FILE *results_file = fopen("sample.ss.chk", "w");
  if (results_file != NULL) {
    fprintf(results_file, "%s\n","p chk sp ss sssp");
    fprintf(results_file, "%s %s %s\n","f", args->filename,args->prob_file);
    fprintf(results_file, "%s %llu %llu %lu %lu\n","g", el.num_vertices, el.num_edges, 0L, 0L);
  }
  // min and max edge weight needs to be reimplemented
  // el.min_edge_weight, el.max_edge_weight);

  call_sssp_args_t sargs;

  double total_elapsed_time = 0.0;

  size_t *edge_traversed =(size_t *) calloc(args->nproblems, sizeof(size_t));
  double *elapsed_time = (double *) calloc(args->nproblems, sizeof(double));

  if (!realloc_adj_list) {
    // Construct the graph as an adjacency list
    hpx_call_sync(HPX_HERE, adj_list_from_edge_list, &sargs.graph,
                  sizeof(sargs.graph), &el, sizeof(el));
  }

  if (args->graph_generator_type == _GRAPH500) {
    // printf("Starting sampling the graph for sources\n");
    sample_graph(&args->problems, args->nproblems,args->edgefactor,el.num_vertices,&sargs.graph);
    // printf("Sampling done\n");
  }

  hpx_bcast_rsync(initialize_sssp_kind, &args->sssp_kind, sizeof(args->sssp_kind));

  if (args->sssp_init_dc_args.num_pq == 0) args->sssp_init_dc_args.num_pq = HPX_THREADS;
  hpx_bcast_rsync(initialize_sssp_kind, &args->sssp_kind, sizeof(args->sssp_kind));
  if (args->sssp_kind == _DC_SSSP_KIND || args->sssp_kind == _DC1_SSSP_KIND) {
    if(args->sssp_init_dc_args.num_pq == 0) args->sssp_init_dc_args.num_pq = HPX_THREADS;
    printf("# priority  queues: %zd\n",args->sssp_init_dc_args.num_pq);
    hpx_bcast_rsync(sssp_init_dc, &args->sssp_init_dc_args, sizeof(args->sssp_init_dc_args));
  }
  if(args->delta > 0) {
    if(args->level > 0) {
      printf("Calling sssp_run_kla action\n");
      hpx_bcast_rsync(sssp_run_kla, NULL, 0);
    } else {
      hpx_bcast_rsync(sssp_run_delta_stepping, NULL, 0);
      sargs.delta = args->delta;
    }
  } else {
      sargs.delta = 0;
  }


  if(args->sssp_kind == KLEVEL_SSSP_KIND) {
    printf("Calling set_klevel action\n");
    hpx_bcast_rsync(set_klevel, &args->level, sizeof(args->level));
    printf("Returned from  broadcasting k\n");
  }

  // printf("About to enter problem loop.\n");

  for (int i = 0; i < args->nproblems; ++i) {
    if (total_elapsed_time > args->time_limit) {
      printf("Time limit of %" SSSP_UINT_PRI " seconds reached. Stopping further SSSP runs.\n", args->time_limit);
      args->nproblems = i;
      break;
    }

    if (realloc_adj_list) {
      // Construct the graph as an adjacency list
      hpx_call_sync(HPX_HERE, adj_list_from_edge_list, &sargs.graph,
                    sizeof(sargs.graph), &el, sizeof(el));
    }

    sargs.source = args->problems[i];
    sargs.k_level = args-> level;
    hpx_time_t now = hpx_time_now();

    // Call the SSSP algorithm
    hpx_addr_t sssp_lco = hpx_lco_future_new(0);
    sargs.termination_lco = sssp_lco;
    if (args->delta == 0 && args->sssp_kind != KLEVEL_SSSP_KIND) {
      printf("Calling SSSP (%s).\n", sssp_kind_str[args->sssp_kind]);
      hpx_call(HPX_HERE, call_sssp, HPX_NULL, &sargs, sizeof(sargs));
    } else if (args->sssp_kind == KLEVEL_SSSP_KIND) { //TODO:merge this condition
      printf("Calling KLA sssp\n");
      hpx_call(HPX_HERE, call_kla_sssp, HPX_NULL, &sargs, sizeof(sargs));
    } else {
      printf("Calling delta-stepping with delta %zu.\n", sargs.delta);
      hpx_call(HPX_HERE, call_delta_sssp, HPX_NULL, &sargs, sizeof(sargs));
    }
    // printf("Waiting for termination LCO at: %zu\n", sssp_lco);
    hpx_lco_wait(sssp_lco);
    // printf("Finished waiting for termination LCO at: %zu\n", sssp_lco);
    hpx_lco_delete(sssp_lco, HPX_NULL);


    double elapsed = hpx_time_elapsed_ms(now)/1e3;
    elapsed_time[i] = elapsed;
    total_elapsed_time += elapsed;

#ifdef GATHER_STAT
    _sssp_statistics *sssp_stat=(_sssp_statistics *)malloc(sizeof(_sssp_statistics));
    hpx_call_sync(sargs.sssp_stat, _print_sssp_stat, sssp_stat,
                  sizeof(_sssp_statistics), sssp_stat,
                  sizeof(_sssp_statistics));
    printf("\nuseful work = %lu,  useless work = %lu\n", sssp_stat->useful_work, sssp_stat->useless_work);

    total_vertex_visit += (sssp_stat->useful_work + sssp_stat->useless_work);
    total_distance_updates += sssp_stat->useful_work;
    total_edge_traversal += sssp_stat->edge_traversal_count;
#endif

#ifdef VERBOSE
    // Action to print the distances of each vertex from the source
    hpx_addr_t vertices = hpx_lco_and_new(el.num_vertices);
    for (int i = 0; i < el.num_vertices; ++i) {
      hpx_addr_t index = hpx_addr_add(sargs.graph, i * sizeof(hpx_addr_t), _index_array_block_size);
      hpx_call(index, _print_vertex_distance_index, vertices, &i, sizeof(i));
    }
    hpx_lco_wait(vertices);
    hpx_lco_delete(vertices, HPX_NULL);
#endif


    hpx_addr_t checksum_lco = HPX_NULL;
    if(args->checksum)hpx_call_sync(sargs.graph, dimacs_checksum,
                                    &checksum_lco, sizeof(checksum_lco),
                                    &el.num_vertices, sizeof(el.num_vertices));
    size_t checksum = 0;
    if(args->checksum) {
      hpx_lco_get(checksum_lco, sizeof(checksum), &checksum);
      hpx_lco_delete(checksum_lco, HPX_NULL);
    }

    //printf("Computing GTEPS...\n");
    hpx_addr_t gteps_lco = HPX_NULL;
    hpx_call_sync(sargs.graph, gteps_calculate, &gteps_lco,
                  sizeof(gteps_lco), &el.num_vertices,
                  sizeof(el.num_vertices));
    size_t gteps = 0;
    hpx_lco_get(gteps_lco, sizeof(gteps), &gteps);
    hpx_lco_delete(gteps_lco, HPX_NULL);
    edge_traversed[i] = gteps;
    //printf("Gteps is %zu\n", gteps);

    printf("Finished problem %d in %.7f seconds (csum = %zu).\n", i, elapsed, checksum);
    if (args->checksum) {
      if (results_file != NULL) 
        fprintf(results_file, "d %zu\n", checksum);
    }

    if (realloc_adj_list) {
      hpx_call_sync(sargs.graph, free_adj_list, NULL, 0, NULL, 0);
    } else {
      reset_adj_list(sargs.graph, &el);
    }
  }

  if (!realloc_adj_list) {
    hpx_call_sync(sargs.graph, free_adj_list, NULL, 0, NULL, 0);
  }

#ifdef GATHER_STAT
  double avg_time_per_source = total_elapsed_time/args->nproblems;
  double avg_vertex_visit  = total_vertex_visit/args->nproblems;
  double avg_edge_traversal = total_edge_traversal/args->nproblems;
  double avg_distance_updates = total_distance_updates/args->nproblems;

  printf("\navg_vertex_visit =  %f, avg_edge_traversal = %f, avg_distance_updates= %f\n", avg_vertex_visit, avg_edge_traversal, avg_distance_updates);

  FILE *fp;
  fp = fopen("perf.ss.res", "a+");

  fprintf(fp, "%s\n","p res sp ss sssp");
  fprintf(fp, "%s %s %s\n","f",args->filename,args->prob_file);
  fprintf(fp,"%s %lu %lu %lu %lu\n","g",el.num_vertices, el.num_edges,el.min_edge_weight, el.max_edge_weight);
  fprintf(fp,"%s %f\n","t",avg_time_per_source);
  fprintf(fp,"%s %f\n","v",avg_vertex_visit);
  fprintf(fp,"%s %f\n","e",avg_edge_traversal);
  fprintf(fp,"%s %f\n","i",avg_distance_updates);
  //fprintf(fp,"%s %s %f\n","c ", "The GTEPS measure is ",(total_edge_traversal/(total_elapsed_time*1.0E9)));

  fclose(fp);
#endif

#ifdef VERBOSE
  printf("\nElapsed time\n");
  for(int i = 0; i < args->nproblems; i++)
    printf("%f\n", elapsed_time[i]);

  printf("\nEdges traversed\n");
  for(int i = 0; i < args->nproblems; i++)
    printf("%zu\n", edge_traversed[i]);
#endif

  printf("\nTEPS statistics:\n");
  double *tm = (double*)malloc(sizeof(double)*args->nproblems);
  double *stats = (double*)malloc(sizeof(double)*9);

  for(int i = 0; i < args->nproblems; i++)
    tm[i] = edge_traversed[i]/elapsed_time[i];

  statistics (stats, tm, args->nproblems);
  PRINT_STATS("TEPS", 1);

  free(tm);
  free(stats);
  free(edge_traversed);
  free(elapsed_time);

  if (args->graph_generator_type == _GRAPH500) {
    free(args->problems);
  }
  hpx_exit(0, NULL);
}


int main(int argc, char *argv[argc]) {
  sssp_uint_t time_limit = 1000;
  int realloc_adj_list = 0;

  sssp_init_dc_args_t sssp_init_dc_args = { .num_pq = 0, .yield_count = 0, .queue_threshold = 50, .freq = 100, .num_elem = 100 };
  sssp_kind_t sssp_kind = _DC_SSSP_KIND;
  size_t delta = 0;
  size_t level = 0;
  termination_t termination = COUNT_TERMINATION;
  graph_generator_type_t graph_generator_type = _DIMACS;
  int SCALE = 16 ;
  int edgefactor = 16;
  bool checksum = true;

  sssp_uint_t nproblems = 2;
  sssp_uint_t *problems;

  // register the actions
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _print_vertex_distance_index, _print_vertex_distance_index_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _print_vertex_distance, _print_vertex_distance_action,
              HPX_POINTER, HPX_SIZE_T);
  HPX_REGISTER_ACTION(HPX_DEFAULT, HPX_MARSHALLED,
              _main, _main_action,
              HPX_POINTER, HPX_SIZE_T);

  int e = hpx_init(&argc, &argv);
  if (e) {
    fprintf(stderr, "HPX: failed to initialize.\n");
    return e;
  }

  int opt = 0;
  while ((opt = getopt(argc, argv, "e:f:g:l:q:s:t:y:z:abcdhkpu?")) != -1) {
    switch (opt) {
    case 'q':
      time_limit = strtoul(optarg, NULL, 0);
      break;
    case 'a':
      realloc_adj_list = 1;
      break;
    case 'b':
      checksum = false;
      break;
    case 'k':
      termination = AND_LCO_TERMINATION;
      break;
    case 'p':
      termination = PROCESS_TERMINATION;
      break;
    case 'd':
      sssp_kind = _DC_SSSP_KIND;
      // TBD: add options to adjust dc parameters
      break;
    case 'u':
      sssp_kind = _DC1_SSSP_KIND;
      break;
    case 'f':
      sssp_init_dc_args.freq = strtoul(optarg,NULL,0);
      break;
    case 'l':
      sssp_init_dc_args.num_pq = strtoul(optarg,NULL,0);
      break;
    case 'c':
      sssp_kind = _CHAOTIC_SSSP_KIND;
      break;
    case 'h':
      _usage(stdout);
      return 0;
    case 'z':
      delta = strtoul(optarg, NULL, 0);
      break;
    case 'g':
      graph_generator_type = _GRAPH500;
      SCALE = strtoul(optarg, NULL, 0);
      break;
    case 's':
      nproblems = strtoul(optarg, NULL, 0);
      break;
    case 't':
      sssp_init_dc_args.queue_threshold = strtoul(optarg, NULL, 0);
      break;
    case 'y':
      sssp_init_dc_args.yield_count = strtoul(optarg, NULL, 0);
      break;
    case 'e':
      sssp_kind = KLEVEL_SSSP_KIND;
      level = strtoul(optarg, NULL, 0);
      break;
    case '?':
    default:
      _usage(stderr);
      return -1;
    }
  }

  argc -= optind;
  argv += optind;

  char *graph_file = NULL;
  char *problem_file = NULL  ;
  if (graph_generator_type == _DIMACS){
    switch (argc) {
    case 0:
      fprintf(stderr, "\nMissing graph (.gr) file.\n");
      _usage(stderr);
      return -1;
    case 1:
      fprintf(stderr, "\nMissing problem specification (.ss) file.\n");
      _usage(stderr);
      return -1;
    default:
      _usage(stderr);
      return -1;
    case 2:
      graph_file = argv[0];
      problem_file = argv[1];
      break;
    }
  }
  if (graph_generator_type == _DIMACS){
    // Read the DIMACS problem specification file
    _read_dimacs_spec(&problem_file, &nproblems, &problems);
  } else { // _GRAPH500
    problems = malloc(1);
  }

  _sssp_args_t args = { .filename = graph_file,
                        .nproblems = nproblems,
                        .problems = problems,
                        .prob_file = problem_file,
                        .time_limit = time_limit,
                        .realloc_adj_list = realloc_adj_list,
            .sssp_kind = sssp_kind,
            .sssp_init_dc_args = sssp_init_dc_args,
            .delta = delta,
            .termination = termination,
            .graph_generator_type = graph_generator_type,
            .scale = SCALE,
            .edgefactor = edgefactor,
            .checksum = checksum,
            .level = level
  };

  e = hpx_run(&_main, NULL, &args, sizeof(args));
  free(problems);
  hpx_finalize();
  return e;
}
