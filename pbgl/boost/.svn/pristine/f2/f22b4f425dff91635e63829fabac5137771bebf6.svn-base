/* ex: set tabstop=4 expandtab: */

// Copyright (C) 2004-2008 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine

// Example usage of breadth_first_search algorithm

// Enable PBGL interfaces to BGL algorithms
#include <boost/graph/use_mpi.hpp>

// Distributed adjacency list
#include <boost/graph/distributed/adjacency_list.hpp>

// Communicate via MPI
#include <boost/graph/distributed/mpi_process_group.hpp>

// Breadth-first search algorithm
//#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/distributed/connected_components_parallel_search.hpp>

// METIS Input
#include <boost/graph/metis.hpp>

// Graphviz Output
#include <boost/graph/distributed/graphviz.hpp>

// For choose_min_reducer
#include <boost/graph/distributed/distributed_graph_utility.hpp>

// Standard Library includes
#include <fstream>
#include <string>
#include <error.h>

#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
#include "defs.h" 

#include <graph500_generator.hpp>

#include <boost/random/linear_congruential.hpp>

#ifdef BOOST_NO_EXCEPTIONS
void
boost::throw_exception(std::exception const& ex)
{
    std::cout << ex.what() << std::endl;
    abort();
}
#endif

typedef double time_type;
std::string print_time(time_type t)
{             
	std::ostringstream out;
	out << std::setiosflags(std::ios::fixed) << std::setprecision(2) << t;
	return out.str();
}             

inline unsigned int __log2p2(unsigned int n) {
	int l = 0;
	while (n >>= 1) l++;
	return l;
}



using namespace boost;
using boost::graph::distributed::mpi_process_group;

bool freeze = true;

//
// BFS test helper
//
template <typename Graph>
double run_cc(mpi_process_group &pg, Graph& g, 
		typename graph_traits<Graph>::vertices_size_type n, bool verify)
{
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
	typedef typename graph_traits<Graph>::edges_size_type edges_size_type;

  // inializae connected components property map
	std::vector<int> local_components_vec(num_vertices(g));
	typedef iterator_property_map<std::vector<int>::iterator, 
					typename property_map<Graph, vertex_index_t>::type> ComponentMap;
	ComponentMap components(local_components_vec.begin(), get(vertex_index, g));

  timer::cpu_times tstart, tend;
  timer::cpu_timer timer;

  tstart.clear();
  tstart = timer.elapsed();

  // run connected components
#if defined CC_PS
	int num_components = connected_components_ps(g, components); 
#else
	int num_components = connected_components(g, components); 
#endif

  tend = timer.elapsed();

  if (verify)
		if (process_id(pg) == 0)
			std::cout << "Components found " << num_components << std::endl;

	if (process_id(pg) == 0) {
		std::cout << "Connected components (CC) search" << std::endl;
		std::cout << "\tTime elapsed " << (double)(tend.wall - tstart.wall)/1000000000 << " sec" << std::endl;
		//std::cout << "\tPerformance " << (m / ((double)(tend.wall - tstart.wall)/1000000000)) / 1000000 << " MTE/s" << std::endl;
		    //CkPrintf("total = %lld, N = %lld(%2f%%), M = %lld(%2f%%), root = %lld\n", total,
				//		        N, 100.0*total/N, M, 100.0*total/M, root);
	}

  // NGE: This is exceedingly arbitrary... remove later
  //if (total <= 100) return -1.;

	return (double)(tend.wall - tstart.wall)/1000000000;
}


std::string ifile, ofile;
bool valid = false;
int scale = 16;
int k = 16;
int num_sources = 1;
bool verify = true;
bool strongscale = true;
//typedef enum {WeakScaling, StrongScaling} ScalingMode;
//ScalingMode scaling_mode = WeakScaling; 
//int weak_scaling_mode = true;

int main(int argc, char* argv[])
{
  boost::mpi::environment env(argc,argv);
  mpi::communicator world;
	double total_cc_time = 0;


	//while (freeze) {}

//-----------------------------------------	
	program_options::options_description opts("Supported options");
	opts.add_options()
		("help,h","print usage message")
		("scale,s",program_options::value(&scale), "graph scale ")
		("k,k",program_options::value(&k), "average degree ")
		("strong,strongscale","strong scaling enabled ")
		("weak,weakscale", "weak scaling enabled ")
		;

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, opts), vm);
	program_options::notify(vm);

	if (vm.count("help")) {
		//if (process_id(pg) == 0)
		if (world.rank() == 0)
			std::cout << opts << std::endl;
		return 0;
	}

	if (vm.count("strong") || vm.count("strongscale")) {
		strongscale = true;
	}

	if (vm.count("weak") || vm.count("weakscale")) {
		strongscale = false;
	}

//	typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>, directedS /*undirectedS*/,
//					/*vertex_descriptor properties=*/property<vertex_distance_t, std::size_t> >
//						Graph;
#if defined GRAPH_UNDIRECTED
	typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>, undirectedS, 
					no_property, no_property>
#elif defined GRAPH_DIRECTED
	typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>, directedS,
					no_property, no_property>
#else
	#error("Graph type (directed/undirected) should be defined")
#endif
				Graph;

	typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef graph_traits<Graph>::edges_size_type edges_size_type;
	typedef graph_traits<Graph>::vertices_size_type vertices_size_type;

	typedef graph500_iterator<Graph> Graph500Iter;

	uint64_t seed64 = 12345;
	// Seed general-purpose RNG
	rand48 gen, synch_gen;
	gen.seed(seed64);
	synch_gen.seed(seed64);


	boost::uniform_int<uint64_t> rand_64(0, std::numeric_limits<uint64_t>::max());
	uint64_t a = rand_64(gen);
	uint64_t b = rand_64(gen);

	//return;

	uint64_t n;
	uint64_t m;

	if (strongscale)
		n = 1 << scale;
	else
		n = (1 << scale) * world.size();

	m = n * k;
	boost::uniform_int<uint64_t> rand_vertex(0, n-1);

	Graph g(n);

	mpi_process_group pg = process_group(g);

	//--------------------------------------------------------------------------------------------	
	// Graph construction block
	{
	  //edges_size_type e_start = trans.rank() * (m + trans.size() - 1) / trans.size();
	  //edges_size_type e_count = (std::min)((m + trans.size() - 1) / trans.size(), m - e_start);

	  uint64_t e_start = world.rank() * (m + world.size() - 1) / world.size(); // FIXME???
	  uint64_t e_count = (std::min)((m + world.size() - 1) / world.size(), m - e_start);

	  Graph500Iter graph500iter(
				strongscale == true ? scale : scale + __log2p2(world.size()),
				e_start, a, b);

	  timer::cpu_times tstart, tend;
	  timer::cpu_timer timer;
		if (process_id(pg) == 0) {
			std::cout << "Connected components Boost running..." << std::endl;
			std::cout << "\tnumber of mpi processes is " << num_processes(pg) << std::endl;
			std::cout << "\tnumber of vertices is " << n << std::endl;
			std::cout << "\tnumber of edges is " << m << std::endl;
			std::cout << "Constructing graph...";

			tstart.clear();
			tstart = timer.elapsed();
		}

		int thrash = 0;
		std::size_t ecounter = 0;

		std::cout << process_id(pg) << ": " << e_start << "," << e_count << "\n";

		for (uint64_t i = 0; i < /*m/num_processes(pg)*/ e_count; i++) {

			if (((double)i/e_count)*100 > thrash) {
				synchronize(pg);
				if (process_id(pg) == 0) {
					tend = timer.elapsed();
					std::cout << thrash << "%, " 
						<< ecounter << " edges added "
						<< (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";
				}
				thrash+=10;
			}


			std::pair<uint64_t,uint64_t> e = graph500iter.dereference();
			add_edge(vertex(e.first, g), vertex(e.second, g), g);
			graph500iter.increment();

			ecounter++;
		}

		/*for (uint64_t i = 0; i < m; i++) {
			std::pair<uint64_t,uint64_t> e = graph500iter.dereference();
			add_edge(vertex(e.first, g), vertex(e.second, g), g);
			graph500iter.increment();
			}*/


		synchronize(pg);

		if (process_id(pg) == 0) {
			std::cout << "...completed" << std::endl;
			std::cout << "Initializtion completed:\n";
		}

	}
	// End of graph construction
	//--------------------------------------------------------------------------------------------	
	// Run breadth-first search
	srandom(1);

	for (unsigned int source_i = 0; source_i < num_sources; ++source_i) {

		//uint64_t root = rand_vertex(synch_gen);
		uint64_t root = ::random() % n;

		if (process_id(pg) == 0)
			std::cout << "root = " << root << std::endl;

		vertex_descriptor current_source = vertex(root, g);

		double cc_time = run_cc<Graph>(pg, g, n, verify);

		if (cc_time == -1.) { // Not enough vertices visited
			--source_i; continue;
		}

		total_cc_time += cc_time;
	}
	//--------------------------------------------------------------------------------------------	

	if (process_id(process_group(g)) == 0) {
		std::cout << "Done." << std::endl;
		std::cout << "Total BFS time for " << num_sources << " sources = "
			<< total_cc_time << " " <<  print_time(total_cc_time) << " (" 
			<< print_time(total_cc_time / num_sources) << " per source)  " << "\n";
		std::cout << "[Final] CPU time used = " << print_time(total_cc_time)  << " seconds\n";
	}

	return 0;
}
