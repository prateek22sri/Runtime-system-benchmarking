#include <boost/graph/use_mpi.hpp> 
#include <boost/mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/distributed/vertex_list_adaptor.hpp>
#include <boost/graph/distributed/dehne_gotz_min_spanning_tree.hpp>
#include <boost/timer/timer.hpp>
#include <boost/program_options.hpp>


#include <iostream>
#include <error.h>

#include "defs.h" 

using namespace boost;
using boost::graph::distributed::mpi_process_group;
using boost::graph::parallel::process_group;
//using namespace boost::graph::distributed;

std::string ifile, ofile;
bool valid = false;

void readGraph_nonalloc(graph_t *G, char *filename);

template<typename ProcessGroup, typename Graph, typename WeightMap, typename EdgeIndexMap,
	typename OutputIterator, typename RankMap, typename ParentMap,
	typename SupervertexMap, typename Vertex, typename EdgeList>
	OutputIterator
boruvka_merge_step(ProcessGroup pg, const Graph& g, WeightMap weight_map, EdgeIndexMap e_index_map,
		OutputIterator out,
		disjoint_sets<RankMap, ParentMap>& dset,
		SupervertexMap supervertex_map,
		std::vector<Vertex>& supervertices,
		EdgeList& edge_list)
{
	typedef typename graph_traits<Graph>::vertex_descriptor
		vertex_descriptor;
	typedef typename graph_traits<Graph>::vertices_size_type
		vertices_size_type;
	typedef typename graph_traits<Graph>::edge_descriptor  edge_descriptor;
	typedef typename EdgeList::iterator                    edge_iterator;
	typedef typename property_traits<WeightMap>::value_type
		weight_type;
	typedef typename property_traits<EdgeIndexMap>::value_type
		e_index_type;
	typedef boost::parallel::detail::untracked_pair<edge_descriptor, 
					weight_type>        w_edge;
	typedef typename property_traits<SupervertexMap>::value_type
		supervertex_index;
	graph::distributed::detail::smaller_weighted_edge min_edge;
	weight_type inf = (std::numeric_limits<weight_type>::max)();

    timer::cpu_times tstart, tend;
    timer::cpu_timer timer;

    tstart.clear();
    tstart = timer.elapsed();

	// Renumber the supervertices
	for (std::size_t i = 0; i < supervertices.size(); ++i)
		put(supervertex_map, supervertices[i], i);


	// BSP-B1: Find local minimum-weight edges for each supervertex
	std::vector<w_edge> candidate_edges(supervertices.size(),
			w_edge(edge_descriptor(), inf));
	/*//if (process_id(pg) == 0)
		for (edge_iterator ei = edge_list.begin(); ei != edge_list.end(); ++ei) {
		weight_type w = get(weight_map, *ei);
		std::cout << process_id(pg) << ": (" << local(source(*ei, g)) << "@" << owner(source(*ei,g)) << ",";
		std::cout << local(target(*ei, g)) << "@" << owner(target(*ei,g)) << ",";
		std::cout << w << ")\n";
		}*/

	for (edge_iterator ei = edge_list.begin(); ei != edge_list.end(); ++ei) {
		weight_type w = get(weight_map, *ei);
		supervertex_index u =
			get(supervertex_map, dset.find_set(source(*ei, g)));
		supervertex_index v =
			get(supervertex_map, dset.find_set(target(*ei, g)));

		if (u != v) {
			candidate_edges[u] = min_edge(candidate_edges[u], w_edge(*ei, w));
			candidate_edges[v] = min_edge(candidate_edges[v], w_edge(*ei, w));
		}
	}

#ifdef PROFILE_ENABLED
    tend = timer.elapsed();
    std::cout << process_id(pg) <<  ": BSP-B1 completed, "
        << (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";

    tstart.clear();
    tstart = timer.elapsed();
#endif

	// BSP-B2 (a): Compute global minimum edges for each supervertex
	//std::cout << process_id(pg)
	//	<< ": candidate_edges.size() = " << candidate_edges.size() << std::endl;
	all_reduce(pg,
			&candidate_edges[0],
			&candidate_edges[0] + candidate_edges.size(),
			&candidate_edges[0], min_edge);

#ifdef PROFILE_ENABLED
    tend = timer.elapsed();
    std::cout << process_id(pg) <<  ": BSP-B2(a) completed, "
        << (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";

    tstart.clear();
    tstart = timer.elapsed();
#endif

	// BSP-B2 (b): Use the edges to compute sequentially the new
	// connected components and emit the edges.
	for (vertices_size_type i = 0; i < candidate_edges.size(); ++i) {
		if (candidate_edges[i].second != inf) {
			edge_descriptor e = candidate_edges[i].first;
			vertex_descriptor u = dset.find_set(source(e, g));
			vertex_descriptor v = dset.find_set(target(e, g));
			if (u != v) {
				// Emit the edge, but cache the weight so everyone knows it
				cache(weight_map, e, candidate_edges[i].second);

				e_index_type e_index = get(e_index_map, e);
				cache(e_index_map, e, e_index);
				*out++ = e;

				/*std::cout << process_id(pg) << ": merged " ; 
				std::cout << "(" << local(source(e,g)) << "@" << owner(source(e,g)) << ",";
				std::cout << local(target(e,g)) << "@" << owner(target(e,g)) << ")\n"; */

				// Link the two supervertices
				dset.link(u, v);

				// Whichever vertex was reparented will be removed from the
				// list of supervertices.
				vertex_descriptor victim = u;
				if (dset.find_set(u) == u) victim = v;
				supervertices[get(supervertex_map, victim)] =
					graph_traits<Graph>::null_vertex();
			}
		}
	}

#ifdef PROFILE_ENABLED
    tend = timer.elapsed();
    std::cout << process_id(pg) <<  ": BSP-B2(b) completed, "
        << (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";

    tstart.clear();
    tstart = timer.elapsed(); 
#endif

	// BSP-B3: Eliminate self-loops
	edge_list.erase(std::remove_if(edge_list.begin(), edge_list.end(),
				graph::distributed::detail::has_same_supervertex(dset, g)),
			edge_list.end());

	// TBD: might also eliminate multiple edges between supervertices
	// when the edges do not have the best weight, but this is not
	// strictly necessary.

	// Eliminate supervertices that have been absorbed
	supervertices.erase(std::remove(supervertices.begin(),
				supervertices.end(),
				graph_traits<Graph>::null_vertex()),
			supervertices.end());

#ifdef PROFILE_ENABLED
    tend = timer.elapsed();
    std::cout << process_id(pg) <<  ": BSP-B3 completed, "
        << (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";
#endif

	return out;
}


// ---------------------------------------------------------------------
// Dense Boruvka MSF algorithm
// ---------------------------------------------------------------------
template<typename Graph, typename WeightMap, typename OutputIterator,
         typename VertexIndexMap, typename EdgeIndexMap, typename RankMap, typename ParentMap,
         typename SupervertexMap>
OutputIterator
dense_boruvka_minimum_spanning_tree(const Graph& g, WeightMap weight_map,
                                    OutputIterator out,
                                    VertexIndexMap v_index_map,
                                    EdgeIndexMap e_index_map,
                                    RankMap rank_map, ParentMap parent_map,
                                    SupervertexMap supervertex_map, 
																		std::vector<typename graph_traits<Graph>::vertex_descriptor> &supervertices,
																		disjoint_sets<RankMap, ParentMap> &dset)
{

  typedef typename graph_traits<Graph>::traversal_category traversal_category;

  BOOST_STATIC_ASSERT((is_convertible<traversal_category*,
                                      vertex_list_graph_tag*>::value));

  typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
  typedef typename graph_traits<Graph>::vertex_descriptor  vertex_descriptor;
  typedef typename graph_traits<Graph>::vertex_iterator    vertex_iterator;
  typedef typename graph_traits<Graph>::edge_descriptor    edge_descriptor;

  // Don't throw away cached edge weights
  weight_map.set_max_ghost_cells(0);
  e_index_map.set_max_ghost_cells(0);

	OutputIterator begin_out = out;

  // Initialize the disjoint sets structures
  //disjoint_sets<RankMap, ParentMap> dset(rank_map, parent_map);
  vertex_iterator vi, vi_end;
  for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    dset.make_set(*vi);

  //std::vector<vertex_descriptor> supervertices;
  supervertices.assign(vertices(g).first, vertices(g).second);

  // Use Kruskal's algorithm to find the minimum spanning forest
  // considering only the local edges. The resulting edges are not
  // necessarily going to be in the final minimum spanning
  // forest. However, any edge not part of the local MSF cannot be a
  // part of the global MSF, so we should have eliminated some edges
  // from consideration.
  std::vector<edge_descriptor> edge_list;
	edge_list.assign(edges(g).first, edges(g).second);


  /*kruskal_minimum_spanning_tree
    (make_vertex_and_edge_range(g, vertices(g).first, vertices(g).second,
                                edges(g).first, edges(g).second),
     std::back_inserter(edge_list),
     boost::weight_map(weight_map).
     vertex_index_map(v_index_map));*/

  // While the number of supervertices is decreasing, keep executing
  // Boruvka steps to identify additional MSF edges. This loop will
  // execute log |V| times.
  vertices_size_type old_num_supervertices;
  int step = 0;
  do {
    old_num_supervertices = supervertices.size();
    if (process_id(process_group(g)) == 0)
        std::cout << "Boruvka step" << step << "...";

    out = boruvka_merge_step(process_group(g), g,
                                     weight_map, e_index_map, out,
                                     dset, supervertex_map, supervertices,
                                     edge_list);
    if (process_id(process_group(g)) == 0)
        std::cout << "completed\n";

  } while (supervertices.size() < old_num_supervertices);

  return out;
}


uint64_t nVertices = 1 << 4;
uint64_t nEdges = 1 << 16;
int freeze = 0;

template<typename Graph, typename WeightMap, typename InputIterator>
void print_mst(const Graph& g, WeightMap weight_map, InputIterator first, InputIterator last) {
	 typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
		while (first != last) {
			vertex_descriptor u = source(*first, g);
			vertex_descriptor v = target(*first, g);
			std::cout << "(" << g.distribution().global(owner(u), local(u)) 
			 << "," << g.distribution().global(owner(v), local(v))
			 << ", weight = " << get(weight_map, *first) << ")\n" ;
			first++;
		}		
}	

template<typename Graph, typename WeightMap, typename EdgeIndexMap>
void print_local_edges(const Graph& g, mpi_process_group& pg, WeightMap weight_map, EdgeIndexMap e_index_map){
	std::cout << "printing edges:\n";
	for (typename Graph::edge_iterator ei = edges(g).first; ei != edges(g).second; ei++) {
		std::cout << process_id(pg) << ": (" 
			<< local(source(*ei, g)) << "@" << owner(source(*ei,g)) << ",";
		std::cout << local(target(*ei, g)) << "@" << owner(target(*ei,g)) << ",";
		std::cout	<< get(weight_map, *ei) << "," << get(e_index_map, *ei);
		std::cout << ")\n";
	}
	std::cout << "end of edge list.\n";
}


template <typename Graph, typename EdgeIndexMap, typename MSF> 
void convertToGraphHPC(Graph & g, forest_t *forest, MSF &msf, EdgeIndexMap &e_index_map) {
	//typedef typename property_map<Graph, edge_index_t>::type EdgeIndexMap;
	//EdgeIndexMap e_index_map = get(edge_index, g);

	forest->p_edge_list = (edge_id_t *)malloc(msf.size()*2 * sizeof(edge_id_t));
	edge_id_t number_of_edges = 0;
	for (vertex_id_t i = 0; i < msf.size(); i++) number_of_edges += msf[i].size();
	forest->edge_id = (edge_id_t *)malloc(number_of_edges * sizeof(edge_id_t));
	forest->p_edge_list[0] = 0;
	forest->p_edge_list[1] = msf[0].size();
	for (vertex_id_t i = 1; i < msf.size(); i++) {
		forest->p_edge_list[2*i] = forest->p_edge_list[2*i-1];
		forest->p_edge_list[2*i +1] = forest->p_edge_list[2*i-1] + msf[i].size();
	}
	int k = 0;
	for (vertex_id_t i = 0; i < msf.size(); i++) {
		for (edge_id_t j = 0; j < msf[i].size(); j++) {
			forest->edge_id[k] = get(e_index_map, msf[i][j]);
			k++;
		}
	}

	forest->numTrees = msf.size();
	forest->numEdges = number_of_edges;
}

template <typename Z>
//void mst(mpi_process_group & pg, graph_t *G, forest_t *trees, Z z) {
void mst(graph_t *G, forest_t *trees, Z z) {

  typedef adjacency_list<listS, 
                         distributedS<mpi_process_group, vecS>,
                         undirectedS,
                         // Vertex properties
                         no_property,
                         // Edge properties
                         property<edge_weight_t, double, property<edge_index_t, std::size_t> > > Graph;
                         //property<edge_weight_t, double> > Graph;
                         //property<edge_weight_t, int> > Graph;


	Graph g(G->n);

    //return;
    mpi_process_group pg = process_group(g);

	//if (process_id(pg) != 0)
    //    freeGraph(G);

	while (freeze) { sleep(5); }

	if (process_id(pg) == 0) {
		std::cout << "MST Boost running..." << std::endl;
		std::cout << "\tnumber of mpi processes is " << num_processes(pg) << std::endl;
		std::cout << "\tnumber of vertices is " << G->n << std::endl;
		std::cout << "\tnumber of edges is " << G->m << std::endl;
		std::cout << "Constructing graph...";
        timer::cpu_times tstart, tend;
        timer::cpu_timer timer;

        tstart.clear();
        tstart = timer.elapsed();

        int thrash = 0;
        std::size_t ecounter = 0;
		for (uint64_t i = 0; i < G->n; i++) {
            if (((double)i/G->n)*100 > thrash) {
                tend = timer.elapsed();
                std::cout << thrash << "%, " 
                    << ecounter << " edges added "
                    << (double)(tend.wall - tstart.wall)/1000000000 << "sec passed\n";
                thrash+=10;
            }
			for (edge_id_t j = G->rowsIndices[i]; j < G->rowsIndices[i+1]; j++) {
				property<edge_weight_t, double, property<edge_index_t, std::size_t> > prop;
				get_property_value(prop, edge_weight) = G->weights[j];
				get_property_value(prop, edge_index) = j;

                if (G->endV[j] > G->n) {
                    std::cout << "Error: vertex index " << G->endV[j]
                       << " is out of range " << std::endl;
                    exit(1);
                }

				add_edge(vertex(i, g), vertex(G->endV[j], g), prop, g);
                ecounter++;
			}
		}
		std::cout << "...completed" << std::endl;
	}

	if (process_id(pg) == 0)
        freeGraph(G);

	synchronize(pg);

	typedef property_map<Graph, edge_weight_t>::type WeightMap;
	WeightMap weight_map = get(edge_weight, g);

	typedef property_map<Graph, edge_index_t>::type EdgeIndexMap;
	EdgeIndexMap e_index_map = get(edge_index, g);

	/*if (process_id(pg) == 0){ 
		print_local_edges(g, pg, weight_map, e_index_map);
	}*/

	typedef graph_traits<Graph>::edge_descriptor edge_descriptor;
	std::vector<edge_descriptor> mst_edges;

  typedef typename graph_traits<Graph>::vertex_descriptor vertex_descriptor;
  typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator;

  std::vector<std::size_t> ranks(num_vertices(make_vertex_list_adaptor(g)));
  std::vector<vertex_descriptor> parents(num_vertices(make_vertex_list_adaptor(g)));
  std::vector<std::size_t> supervertices(num_vertices(make_vertex_list_adaptor(g)));

	typedef typename graph::detail::default_global_index_map<Graph>::type MapIndex;
	MapIndex map_i = get(vertex_index, make_vertex_list_adaptor(g));

  std::vector<vertex_descriptor> sv;

	typedef iterator_property_map<typename std::vector<std::size_t>::iterator, 
					MapIndex,
					typename std::iterator_traits<typename std::vector<std::size_t>::iterator>::value_type,
					typename std::iterator_traits<typename std::vector<std::size_t>::iterator>::reference
				 	> RankMap;

	typedef iterator_property_map<typename std::vector<vertex_descriptor>::iterator, 
					MapIndex,
					typename std::iterator_traits<typename std::vector<vertex_descriptor>::iterator>::value_type,
					typename std::iterator_traits<typename std::vector<vertex_descriptor>::iterator>::reference
				 	> ParentMap;

  disjoint_sets<RankMap, ParentMap> dset(make_iterator_property_map(ranks.begin(), map_i), 
			make_iterator_property_map(parents.begin(), map_i));
  vertex_iterator vi, vi_end;
  for (boost::tie(vi, vi_end) = vertices(g); vi != vi_end; ++vi)
    dset.make_set(*vi);

	if (process_id(pg) == 0){ 
		std::cout << "dense_boruvka_minimum_spanning_tree running...";
	}

	timer::cpu_times tstart, tend;
	timer::cpu_timer timer;

	tstart.clear();
	tstart = timer.elapsed();

  dense_boruvka_minimum_spanning_tree(make_vertex_list_adaptor(g), weight_map, std::back_inserter(mst_edges));
/*	dense_boruvka_minimum_spanning_tree
           (make_vertex_list_adaptor(g), weight_map, std::back_inserter(mst_edges), map_i, e_index_map,
            make_iterator_property_map(ranks.begin(), map_i),
            make_iterator_property_map(parents.begin(), map_i),
            make_iterator_property_map(supervertices.begin(), map_i), sv, dset);
*/
	tend = timer.elapsed();

	if (process_id(pg) == 0)
		std::cout << "...completed" << std::endl;

	if (process_id(pg) == 0) {
		std::cout << "Minimum spanning tree" << std::endl;
		std::cout << "\tNumber of trees in the forest is " << sv.size() << std::endl;
		std::cout << "\tNumber of edges is " << mst_edges.size() << std::endl;
		std::cout << "\tTime elapsed " << (double)(tend.wall - tstart.wall)/1000000000 << " sec" << std::endl;
		std::cout << "\tPerformance " << (G->m / ((double)(tend.wall - tstart.wall)/1000000000)) / 1000000 << " MTE/s" << std::endl;

		if (valid) {
			std::vector<std::vector<edge_descriptor> > mst_edges_by_tree(sv.size());
			for (std::vector<edge_descriptor>::iterator ei = mst_edges.begin(); ei != mst_edges.end(); ei++) {
				std::size_t tree_id = get(make_iterator_property_map(supervertices.begin(), map_i), 
						dset.find_set(target(*ei,g)));
				mst_edges_by_tree[tree_id].push_back(*ei);
			}
			
			/*for (int i = 0; i < mst_edges_by_tree.size(); i++) {
				std::cout << "Tree " << i << ", size = " << mst_edges_by_tree[i].size() <<
					", root = " << local(sv[i]) << "@" << owner(sv[i]) << "\n";
				for (std::vector<edge_descriptor>::iterator ei = mst_edges_by_tree[i].begin(); ei != mst_edges_by_tree[i].end(); ei++)
					std::cout << "mst edge: (" 
						<< local(source(*ei,g)) << "@" << owner(source(*ei,g))   
						<< "," << local(target(*ei,g)) << "@" << owner(target(*ei,g)) 
						<< "," 
						<< get(weight_map, *ei) 
						<< "," 
						<< get(e_index_map, *ei) 
						<< ") -- " 
						<< get(make_iterator_property_map(supervertices.begin(), map_i), dset.find_set(source(*ei,g))) << "," 
						<< get(make_iterator_property_map(supervertices.begin(), map_i), dset.find_set(target(*ei,g))) << "\n";
			}*/
			convertToGraphHPC(g, trees, mst_edges_by_tree, e_index_map); 
		}
	}
}


int main(int argc, char **argv) {
	mpi::environment env(argc, argv);
    mpi::communicator world;
	//mpi_process_group pg;
	
	program_options::options_description opts("Supported options");
	opts.add_options()
		("help,h","print usage message")
		("input,i",program_options::value(&ifile), "input file")
		("output,o",program_options::value(&ofile), "output file")
		("validation,v",program_options::bool_switch(&valid), "enable validation")	;

	program_options::variables_map vm;
	program_options::store(program_options::parse_command_line(argc, argv, opts), vm);
	program_options::notify(vm);

	if (vm.count("help")) {
		//if (process_id(pg) == 0)
		if (world.rank() == 0)
			std::cout << opts << std::endl;
		return 0;
	}

	std::cout << "ifile = " << ifile << std::endl;
	std::cout << "ofile = " << ofile << std::endl;

	// load graph
	graph_t G;

	//if (process_id(pg) == 0) 
    if (world.rank() == 0)
		readGraph(&G, (char *)ifile.c_str()); 
    else
        readGraph_nonalloc(&G, (char *)ifile.c_str()); 

	forest_t trees;
	//mst(pg, &G, &trees, 0);
	mst(&G, &trees, 0);

	//if ((process_id(pg) == 0) && (valid))
	if ((world.rank() == 0) && (valid))
		write_output_information(&trees, (char *)ofile.c_str());
	
	return 0;
}


void write_output_information(forest_t *trees, char *filename)
{

	std::cout << "trees->numEdges = " << trees->numEdges << std::endl;
    FILE *F = fopen(filename, "wb");
    assert(fwrite(&trees->numTrees, sizeof(vertex_id_t), 1, F) == 1);
    assert(fwrite(&trees->numEdges, sizeof(edge_id_t), 1, F) == 1);
    assert(fwrite(trees->p_edge_list, sizeof(edge_id_t), 2*trees->numTrees, F) == 2*trees->numTrees);
    assert(fwrite(trees->edge_id, sizeof(edge_id_t), trees->numEdges, F) == trees->numEdges);
    fclose(F);
}

void readGraph_nonalloc(graph_t *G, char *filename)
{
    uint8_t align;
    FILE *F = fopen(filename, "rb");
    if (!F) error(EXIT_FAILURE, 0, "Error in opening file %s", filename);
    
    assert(fread(&G->n, sizeof(vertex_id_t), 1, F) == 1);
    G->scale = log(G->n) / log (2);
    
    assert(fread(&G->m, sizeof(edge_id_t), 1, F) == 1);
    assert(fread(&G->directed, sizeof(bool), 1, F) == 1);
    assert(fread(&align, sizeof(uint8_t), 1, F) == 1);
    fclose(F);
}
