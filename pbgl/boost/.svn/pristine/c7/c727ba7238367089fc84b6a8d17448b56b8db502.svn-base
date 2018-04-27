#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/property_map/parallel/distributed_property_map.hpp>
#include <boost/property_map/parallel/global_index_map.hpp>
#include <boost/graph/distributed/page_rank.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random.hpp>
#include <vector>
#include <iostream>
#include <stdlib.h>

#ifdef CSR
#include <boost/graph/distributed/compressed_sparse_row_graph.hpp>
#endif

#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>
#include <graph500_generator.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/graph/parallel/algorithm.hpp>

using namespace boost;
using boost::graph::distributed::mpi_process_group;

inline unsigned int __log2p2(unsigned int n) {
	int l = 0;
	while (n >>= 1) l++;
	return l;
}

bool close_to(double x, double y)
{
  double diff = x - y;
  if (diff < 0) diff = -diff;
  double base = (y == 0? x : y);
  if (base != 0) return diff / base < 0.01;
  else return true;
}

// Make convenient labels for the vertices
template <typename Graph>
double run_page_rank(mpi_process_group &pg, Graph &g, 
		typename graph_traits<Graph>::vertices_size_type n, double p, int iterations)
{
  typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  std::vector<double> rank(num_vertices(g));

  timer::cpu_times tstart, tend;
  timer::cpu_timer timer;

  tstart.clear();
  tstart = timer.elapsed();

	// run pagerank
	page_rank(g,
			make_iterator_property_map(rank.begin(), get(vertex_index, g)), 
			boost::graph::n_iterations(iterations),
			0.85, n);

  tend = timer.elapsed();

	if (process_id(pg) == 0) {
		std::cout << "\tTime elapsed " << (double)(tend.wall - tstart.wall)/1000000000 << " sec" << std::endl;
	}

	return (double)(tend.wall - tstart.wall)/1000000000;
}


typedef double time_type;
std::string print_time(time_type t)
{             
	std::ostringstream out;
	out << std::setiosflags(std::ios::fixed) << std::setprecision(2) << t;
	return out.str();
}             

int scale = 16;
int k = 16;
bool strongscale = true;
int iters = 4;

int main(int argc, char* argv[])
{
  boost::mpi::environment env(argc,argv);
  mpi::communicator world;

	program_options::options_description opts("Supported options");
	opts.add_options()
		("help,h","print usage message")
		("scale,s",program_options::value(&scale), "graph scale ")
		("iters,i",program_options::value(&iters), "number of iterations ")
		("k,k",program_options::value(&k), "average degree ")
		("strong,strongscale","strong scaling enabled ")
		("weak,weakscale", "weak scaling enabled ")
		;

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, opts), vm);
	program_options::notify(vm);

	if (vm.count("help")) {
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

	typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>, directedS /*undirectedS*/,
					/*vertex_descriptor properties=*/property<vertex_distance_t, std::size_t> >
						Graph;

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
		n = (1 << scale) * world.size();
	//std::cout << "n = " << n << "\n";

	m = n * k;
	boost::uniform_int<uint64_t> rand_vertex(0, n-1);

	//Graph g(N, trans);
	// Load the graph using the default distribution
	//Graph g(reader.begin(), reader.end(), reader.num_vertices(), trans);

	Graph g(n);

	mpi_process_group pg = process_group(g);

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
	  if (world.rank() == 0) {
			std::cout << "PageRank PBGL running..." << std::endl;
			std::cout << "\tnumber of mpi processes is " << world.size() << std::endl;
			std::cout << "\tnumber of vertices is " << n << std::endl;
			std::cout << "\tnumber of edges is " << m << std::endl;
			std::cout << "Constructing graph...";

			tstart.clear();
			tstart = timer.elapsed();
		}

		int thrash = 0;
		std::size_t ecounter = 0;

		for (uint64_t i = 0; i < e_count; i++) {

			if (((double)i/e_count)*100 > thrash) {
				synchronize(pg);
				if (world.rank() == 0) {
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

		if (world.rank() == 0) {
			std::cout << "...completed" << std::endl;
			std::cout << "Initializtion completed:\n";
		}
	}

	// End of graph construction
	//--------------------------------------------------------------------------------------------	

	// Run page rank
	double total_pr_time = 
		run_page_rank<Graph>(pg, g, n, 0.01, iters);

	//--------------------------------------------------------------------------------------------	
  if (world.rank() == 0) {
		std::cout << "Done." << std::endl;
		std::cout << "Total PageRank time " <<  print_time(total_pr_time) << 
			 print_time(total_pr_time / iters) << " per iteration\n";
		std::cout << "[Final] CPU time used = " << print_time(total_pr_time)  << " seconds\n";
	}

  return 0;
}
