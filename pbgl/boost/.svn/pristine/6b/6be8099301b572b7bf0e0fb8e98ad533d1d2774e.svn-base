#ifndef __printer_hpp__
#define __printer_hpp__
#include <boost/property_map/parallel/global_index_map.hpp>
#include <boost/graph/parallel/properties.hpp>

using namespace boost;

template <class Graph>
class graph_printer {
	public:
		graph_printer(Graph &g) : g(g) {}
		void run() {

			typedef typename property_map<Graph, vertex_index_t>::type VertexIndexMap;
			typedef typename property_map<Graph, vertex_global_t>::type VertexGlobalMap;

			parallel::global_index_map<VertexIndexMap, VertexGlobalMap>
				global_index(g.transport(), num_vertices(g), get(vertex_index, g),
						get(vertex_global, g));

			typename boost::property_map<Graph, vertex_global_t>::type 
				index_map = get(vertex_global, g);
			BGL_FORALL_VERTICES_T(v, g, Graph) {
				std::cout << "vid:" << get(global_index, v);
				std::cout << "(";
        BGL_FORALL_ADJ_T(v, u, g, Graph) {
					std::cout << get(global_index, u) << ",";
				}
				std::cout << ")\n";
			}
		}
	private:
		Graph&				g;
};

#endif
