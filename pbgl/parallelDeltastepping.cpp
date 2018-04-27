// Copyright (C) 2004-2006 The Trustees of Indiana University.

// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  Authors: Douglas Gregor
//           Andrew Lumsdaine

// Example usage of dijkstra_shortest_paths algorithm

// Enable PBGL interfaces to BGL algorithms

#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/distributed/delta_stepping_shortest_paths.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/metis.hpp>
#include <boost/graph/distributed/graphviz.hpp>
#include <boost/property_map/property_map.hpp>
#include <fstream>
#include <string>

#ifdef BOOST_NO_EXCEPTIONS
	void
boost::throw_exception(std::exception const& ex)
{
	std::cout << ex.what() << std::endl;
	abort();
}
#endif
using namespace std;
using namespace boost;
using boost::graph::distributed::mpi_process_group;

/* An undirected, weighted graph with distance values stored on the
   vertices. */
typedef adjacency_list<vecS, distributedS<mpi_process_group, vecS>, undirectedS,
		/*Vertex properties=*/property<vertex_distance_t, float>,
		/*Edge properties=*/property<edge_weight_t, float> >
		Graph;

		int main(int argc, char *argv[]) {
			boost::mpi::environment env(argc, argv);




            // Parse command-line options
			const char *filename = "file12.gr";
			if (argc > 1) filename = argv[1];

			// Open the METIS input file
			std::ifstream in(filename);
			graph::metis_reader reader(in);


			// Load the graph using the default distribution
			Graph g(reader.begin(), reader.end(), reader.weight_begin(),
					reader.num_vertices());

//			cout << "For processor " << process_id(process_group(g)) <<" start = "<<reader.begin()<<" and end = "<<reader.end();

			// Get vertex 0 in the graph
			graph_traits<Graph>::vertex_descriptor start = vertex(0, g);

            double st = MPI_Wtime();
			// Compute shortest paths from vertex 0
            graph::distributed::delta_stepping_shortest_paths(g, start, dummy_property_map(),
                                                              get(vertex_distance, g),
                                                              get(edge_weight, g));
            double et = MPI_Wtime();
            if (process_id(process_group(g)) == 0) {
                cout << "Time taken is " << et - st << endl;
            }
			// Output a Graphviz DOT file
//			std::string outfile = filename;
//			if (argc > 2)
//				outfile = argv[2];
//			else {
//				int i = outfile.rfind('.');
//				if (i != std::string::npos)
//					outfile.erase(outfile.begin() + i, outfile.end());
//				outfile += "-deltaStepping.dot";
//			}
//
//			if (process_id(process_group(g)) == 0) {
//				std::cout << "Writing GraphViz output to " << outfile << "... ";
//				std::cout.flush();
//			}
//			write_graphviz(outfile, g,
//					make_label_writer(get(vertex_distance, g)),
//					make_label_writer(get(edge_weight, g)));
//			if (process_id(process_group(g)) == 0)
//				std::cout << "Done." << std::endl;

			return 0;
		}
