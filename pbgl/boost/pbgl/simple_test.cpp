#include <boost/graph/use_mpi.hpp> 
#include <boost/mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/vertex_list_adaptor.hpp>
#include <boost/graph/distributed/dehne_gotz_min_spanning_tree.hpp>
#include <boost/timer/timer.hpp>
#include <iostream>

using namespace boost;
using boost::graph::distributed::mpi_process_group;
using boost::graph::parallel::process_group;

std::size_t nVertices = (1 << 20);
std::size_t nEdges = (1 << 28);

int main(int argc, char **argv) {
	boost::mpi::environment env(argc, argv);
	mpi_process_group pg;
	
  typedef adjacency_list<listS, 
                         distributedS<mpi_process_group, vecS>,
                         undirectedS,
                         // Vertex properties
                         no_property,
                         // Edge properties
                         property<edge_weight_t, int> > Graph;
	Graph g(nVertices);

	if (process_id(pg) == 0) {
		std::cout << "MST Boost running..." << std::endl;
		std::cout << "\tnumber of mpi processes is " << num_processes(pg) << std::endl;
		std::cout << "\tnumber of vertices is " << nVertices << std::endl;
		std::cout << "\tnumber of edges is " << /*nEdges*/ nEdges << std::endl;
		std::cout << "Constructing graph...";
        timer::cpu_times tstart, tend;
        timer::cpu_timer timer;

        tstart.clear();
        tstart = timer.elapsed();

        int thrash = 0;
        std::size_t ecounter = 0;

		for (std::size_t i = 0; i < nEdges; i++) {
            //if (((double)i/nEdges)*100 > thrash) {
            if (i % 10000 == 0) {
                tend = timer.elapsed();
                std::cout << ((double)i/nEdges)*100 << "%, " 
                    << ecounter << " edges added "
                    << (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";
                thrash+=10;
            }
			uint64_t s = rand() % nVertices;
			uint64_t d = rand() % nVertices;
			uint64_t w = rand();
			add_edge(vertex(s, g), vertex(d, g), w, g);
            ecounter++;
		}
		std::cout << "...completed" << std::endl;
	}

	synchronize(pg);

	/*typedef property_map<Graph, edge_weight_t>::type WeightMap;
	WeightMap weight_map = get(edge_weight, g);

	if (process_id(pg) == atoi(argv[1])){ 
		std::cout << "printing vertices:\n";
		Graph::vertex_iterator v, v_end;
		for (boost::tie(v,v_end) = vertices(g); v != v_end; v++) {
			std::cout << process_id(pg) << ": (" 
				<< local(*v) << "@" << owner(*v) << ")\n";
		}
		std::cout << "end of vertex list.\n";
		std::cout << "printing edges:\n";
		for (typename Graph::edge_iterator ei = edges(g).first; ei != edges(g).second; ei++) {
			std::cout << process_id(pg) << ": (" 
				<< local(source(*ei, g)) << "@" << owner(source(*ei,g)) << ","
 			  << local(target(*ei, g)) << "@" << owner(target(*ei,g)) << ","
				<< get(weight_map, *ei) << ")\n";
		}
		std::cout << "end of edge list.\n";
	}*/

	return 0;
}
