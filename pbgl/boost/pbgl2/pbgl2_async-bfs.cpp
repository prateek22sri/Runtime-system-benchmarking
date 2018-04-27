// Copyright (C) 2004-2012 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Nicholas Edmonds
//           Douglas Gregor
//           Andrew Lumsdaine

// Example usage of breadth_first_search algorithm

#include <am++/am++.hpp>

// Communicate via MPI
#include <am++/mpi_transport.hpp>

// Enable PBGL interfaces to BGL algorithms
#include <boost/graph/use_mpi.hpp>

// Breadth-first search algorithm
#include <boost/graph/breadth_first_search.hpp>

// Distributed adjacency list
#include <boost/graph/distributed/adjacency_list.hpp>

// METIS Input
#include <boost/graph/metis.hpp>

// Graphviz Output
#include <boost/graph/distributed/graphviz.hpp>

#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
#include <graph500_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/graph/parallel/algorithm.hpp>

// Standard Library includes
#include <fstream>
#include <string>

#include "common.h"

#ifdef BOOST_NO_EXCEPTIONS
void
boost::throw_exception(std::exception const& ex)
{
    std::cout << ex.what() << std::endl;
    abort();
}
#endif

inline unsigned int __log2p2(unsigned int n) {
	int l = 0;
	while (n >>= 1) l++;
	return l;
}


typedef int weight_type;
// Edge property bundle
struct WeightedEdge {
  WeightedEdge(weight_type weight = 0) : weight(weight) { }
  weight_type weight;
};

// Vertex property bundle
struct VertexProperties {
  VertexProperties(int d = 0) : distance(d) { }
  int distance;
};

// Define MPI datatypes for bundled properties so AM++ can send messages containing them
namespace amplusplus {
  template<>
  struct make_mpi_datatype<WeightedEdge> : make_mpi_datatype_base {
    make_mpi_datatype<weight_type> dt1;
    scoped_mpi_datatype dt;
    make_mpi_datatype(): dt1() {
      int blocklengths[1] = {1};
      MPI_Aint displacements[1];
      WeightedEdge test_object;
      MPI_Aint test_object_ptr;
      MPI_Get_address(&test_object, &test_object_ptr);
      MPI_Get_address(&test_object.weight, &displacements[0]);
      displacements[0] -= test_object_ptr;
      MPI_Datatype types[1] = {dt1.get()};
      MPI_Type_create_struct(1, blocklengths, displacements, types, dt.get_ptr());
      MPI_Type_commit(dt.get_ptr());
    }
    MPI_Datatype get() const {return dt;}
  };

  template<>
  struct make_mpi_datatype<VertexProperties> : make_mpi_datatype_base {
    make_mpi_datatype<int> dt1;
    scoped_mpi_datatype dt;
    make_mpi_datatype(): dt1() {
      int blocklengths[1] = {1};
      MPI_Aint displacements[1];
      VertexProperties test_object;
      MPI_Aint test_object_ptr;
      MPI_Get_address(&test_object, &test_object_ptr);
      MPI_Get_address(&test_object.distance, &displacements[0]);
      displacements[0] -= test_object_ptr;
      MPI_Datatype types[1] = {dt1.get()};
      MPI_Type_create_struct(1, blocklengths, displacements, types, dt.get_ptr());
      MPI_Type_commit(dt.get_ptr());
    }
    MPI_Datatype get() const {return dt;}
  };
}

using namespace boost;
//using namespace amplusplus;

//
// BFS test helper
//
template <typename Graph, typename ColorMap, typename DistanceMap>
double run_bfs(amplusplus::transport &trans, amplusplus::transport &barrier_trans, 
		Graph& g,	typename graph_traits<Graph>::vertex_descriptor current_source,
		ColorMap & color,
		DistanceMap & distance,
		typename graph_traits<Graph>::vertices_size_type n, bool verify)
{
	typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;
	typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
	typedef typename graph_traits<Graph>::edges_size_type edges_size_type;
	typedef color_traits<typename property_traits<ColorMap>::value_type> Color;


  vertex_descriptor &start = current_source;

	//typedef typename property_map<Graph, int VertexProperties::*>::type DistanceMap;
  //DistanceMap distance = get(&VertexProperties::distance, g);

	if (trans.rank() == 0) {
		std::cout << "Breadth-first search" << std::endl;
	}

  // Initialize distances to infinity and set reduction operation to 'min'
  BGL_FORALL_VERTICES_T(v, g, Graph) {
    put(distance, v, (std::numeric_limits<std::size_t>::max)());
    //put(color, v, (std::numeric_limits<std::size_t>::max)());
  }

	{ amplusplus::scoped_epoch epoch(barrier_trans); }

  //FIXME!!: distance.set_reduce(boost::graph::distributed::choose_min_reducer<std::size_t>());

  // inializae root node with zero distance
	{ amplusplus::scoped_epoch epoch(trans); 
		//put(distance, start, 0);
	}
	//{ amplusplus::scoped_epoch epoch(barrier_trans); }
	//return 0;


  timer::cpu_times tstart, tend;
  timer::cpu_timer timer;

  tstart.clear();
  tstart = timer.elapsed();

  // run breadth-first search
	//breadth_first_search
  // (g, start,
  //   visitor(make_bfs_visitor(record_distances(distance, on_tree_edge()))));
  boost::graph::distributed::async_breadth_first_search<
		Graph, DistanceMap > bfs(g, distance);
  bfs.run(start);

  tend = timer.elapsed();

  vertices_size_type visited = 0;
  BGL_FORALL_VERTICES_T(v, g, Graph) {
	if (get(distance, v) < (std::numeric_limits<std::size_t>::max)())
		//if (get(color, v) == Color::black()) 
			++visited;
  }

	vertices_size_type total = boost::parallel::all_reduce_sum(barrier_trans, visited);

  //if (verify)
	//	if (process_id(pg) == 0)
	//		std::cout << "Visited " << total << " vertices of " << n << std::endl;

	if (trans.rank() == 0) {
		std::cout << "\tTime elapsed " << (double)(tend.wall - tstart.wall)/1000000000 << " sec" << std::endl;
		//std::cout << "\tPerformance " << (m / ((double)(tend.wall - tstart.wall)/1000000000)) / 1000000 << " MTE/s" << std::endl;
		std::cout << "\ttotal = " << total << std::endl;
		    //CkPrintf("total = %lld, N = %lld(%2f%%), M = %lld(%2f%%), root = %lld\n", total,
				//		        N, 100.0*total/N, M, 100.0*total/M, root);
	}

  // NGE: This is exceedingly arbitrary... remove later
  if (total <= 0.25 * n) return -1.;

	return (double)(tend.wall - tstart.wall)/1000000000;
}


typedef double time_type;
std::string print_time(time_type t)
{             
	std::ostringstream out;
	out << std::setiosflags(std::ios::fixed) << std::setprecision(2) << t;
	return out.str();
}             

/* An undirected graph with distance values stored on the vertices. */
typedef adjacency_list<vecS, distributedS<vecS>, directedS,
                       VertexProperties, WeightedEdge>
  Graph;

#define N 16

bool valid = false;
int scale = 16;
int k = 16;
int num_sources = 1;
bool verify = true;
bool strongscale = true;

int main(int argc, char* argv[])
{
  amplusplus::environment env = amplusplus::mpi_environment(argc, argv);
  amplusplus::transport trans = env.create_transport();
	trans.set_nthreads(1);
	amplusplus::transport barrier_trans = trans.clone();

	{ amplusplus::scoped_epoch epoch(trans); }

	//std::cout << trans.rank() << "done\n";
	//return 0;

	double total_bfs_time = 0;

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
		if (trans.rank() == 0)
			std::cout << opts << std::endl;
		return 0;
	}

	if (vm.count("strong") || vm.count("strongscale")) {
		strongscale = true;
	}

	if (vm.count("weak") || vm.count("weakscale")) {
		strongscale = false;
	}

	// Graph generation
	typedef graph500_iterator<Graph> Graph500Iter;
	uint64_t seed64 = 12345;
	// Seed general-purpose RNG
	rand48 gen, synch_gen;
	gen.seed(seed64);
	synch_gen.seed(seed64);
	boost::uniform_int<uint64_t> rand_64(0, std::numeric_limits<uint64_t>::max());
	uint64_t a = rand_64(gen);
	uint64_t b = rand_64(gen);

	uint64_t n;
	uint64_t m;

	if (strongscale)
		n = 1 << scale;
	else
		n = (1 << scale) * trans.size();
	//std::cout << "n = " << n << "\n";

	m = n * k;
	boost::uniform_int<uint64_t> rand_vertex(0, n-1);

	//Graph g(N, trans);
  // Load the graph using the default distribution
  //Graph g(reader.begin(), reader.end(), reader.num_vertices(), trans);
  Graph g(n, trans);

	{
	  //edges_size_type e_start = trans.rank() * (m + trans.size() - 1) / trans.size();
	  //edges_size_type e_count = (std::min)((m + trans.size() - 1) / trans.size(), m - e_start);

	  uint64_t e_start = trans.rank() * (m + trans.size() - 1) / trans.size(); // FIXME???
	  uint64_t e_count = (std::min)((m + trans.size() - 1) / trans.size(), m - e_start);

	  Graph500Iter graph500iter(
				strongscale == true ? scale : scale + __log2p2(trans.size()),
				e_start, a, b);

	  timer::cpu_times tstart, tend;
	  timer::cpu_timer timer;
	  if (trans.rank() == 0) {
			std::cout << "BFS PBGL2 running..." << std::endl;
			std::cout << "\tnumber of mpi processes is " << trans.size() << std::endl;
			std::cout << "\tnumber of vertices is " << n << std::endl;
			std::cout << "\tnumber of edges is " << m << std::endl;
			std::cout << "Constructing graph...";

			tstart.clear();
			tstart = timer.elapsed();
		}

		int thrash = 0;
		std::size_t ecounter = 0;

		//std::cout << trans.rank() << ": " << e_start << "," << e_count << "\n";

		{ amplusplus::scoped_epoch epoch(trans); 

			for (uint64_t i = 0; i < e_count; i++) {

				if (((double)i/e_count)*100 > thrash) {

					{ amplusplus::scoped_epoch epoch(barrier_trans); }

					if (trans.rank() == 0) {
						tend = timer.elapsed();
						std::cout << thrash << "%, " 
							<< ecounter << " edges added "
							<< (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";
					}
					thrash+=10;
				}

				std::pair<uint64_t,uint64_t> e = graph500iter.dereference();

				//if (trans.rank() == 0)
					add_edge(vertex(e.first, g), vertex(e.second, g), g);

				graph500iter.increment();

				ecounter++;
			}
		}

		/*for (uint64_t i = 0; i < m; i++) {
			std::pair<uint64_t,uint64_t> e = graph500iter.dereference();
			add_edge(vertex(e.first, g), vertex(e.second, g), g);
			graph500iter.increment();
			}*/

		//synchronize(pg);
		{ amplusplus::scoped_epoch epoch(barrier_trans); }

		if (trans.rank() == 0) {
			std::cout << "...completed" << std::endl;
			std::cout << "Initializtion completed:\n";
		}
	}

	// End of graph construction
	//--------------------------------------------------------------------------------------------	
	// Run breadth-first search
	typedef graph_traits<Graph>::vertex_descriptor vertex_descriptor;
	typedef graph_traits<Graph>::edges_size_type edges_size_type;
	typedef graph_traits<Graph>::vertices_size_type vertices_size_type;

	//typedef typename property_map<Graph, int VertexProperties::*>::type DistanceMap;
  //DistanceMap distance = get(&VertexProperties::distance, g);
  std::vector<std::size_t> distanceS(num_vertices(g), std::numeric_limits<std::size_t>::max());
	typedef iterator_property_map<std::vector<std::size_t>::iterator,
						 property_map<Graph, vertex_index_t>::type> DistanceMap;
	DistanceMap distance(distanceS.begin(), get(vertex_index, g));

	typedef two_bit_color_map<property_map<Graph, vertex_index_t>::type> ColorMap;
	ColorMap color(num_vertices(g), get(vertex_index, g));
	srandom(1);

	for (unsigned int source_i = 0; source_i < num_sources; ++source_i) {

		//uint64_t root = rand_vertex(synch_gen);
		uint64_t root = stdlib_random() % n;

		if (trans.rank() == 0)
			std::cout << "root = " << root << std::endl;

		vertex_descriptor current_source = vertex(root, g);

		//{ amplusplus::scoped_epoch epoch(trans); 
			//amplusplus::transport trans = trans.clone();

			double bfs_time = 
				run_bfs<Graph>(trans, barrier_trans, g, current_source, color, distance, n, verify);

			if (bfs_time == -1.) { // Not enough vertices visited
				--source_i; continue;
			}
			total_bfs_time += bfs_time;

		//}
	}

	//--------------------------------------------------------------------------------------------	
  if (trans.rank() == 0) {
		std::cout << "Done." << std::endl;
		std::cout << "Total BFS time for " << num_sources << " sources = "
			<< total_bfs_time << " " <<  print_time(total_bfs_time) << " (" 
			<< print_time(total_bfs_time / num_sources) << " per source)  " << "\n";
		std::cout << "[Final] CPU time used = " << print_time(total_bfs_time)  << " seconds\n";
	}

  return 0;
}
