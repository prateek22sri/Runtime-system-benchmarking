#include <GraphLib.h>
#include <common.h>

class BetweenCentrVertex;
class BetweenCentrEdge;
class BetweenCentrGraph;

#include "charm_bc.decl.h"

class BetweenCentrGraph : public GraphLib::Graph<
	BetweenCentrVertex,
	BetweenCentrEdge,
	CProxy_BetweenCentrVertex,
	GraphLib::TransportType::Charm> {
public:
	bool directed;
	CmiUInt8 nVertex;
public:
	BetweenCentrGraph(bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BetweenCentrVertex, 
				BetweenCentrEdge,
				CProxy_BetweenCentrVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	BetweenCentrGraph(CmiUInt8 nVertex, bool directed = true) : 
		nVertex(nVertex),
	  directed(directed),	
		GraphLib::Graph<
				BetweenCentrVertex, 
				BetweenCentrEdge,
				CProxy_BetweenCentrVertex, 
				GraphLib::TransportType::Charm >(nVertex)	
		{}
	BetweenCentrGraph(const CProxy_BetweenCentrVertex & g, bool directed = true) : 
		directed(directed),
		GraphLib::Graph<
				BetweenCentrVertex, 
				BetweenCentrEdge,
				CProxy_BetweenCentrVertex, 
				GraphLib::TransportType::Charm >(g)	
		{}
	void startMultiBFSPhase() {
		//g.make_root();
		int l = 64;
		for (CmiUInt8 i = 0; i < nVertex; i += l) { 
			CkPrintf("multi-bfs: %d/%d\n",i,nVertex);
			for (int j = 0; j < l; j++)
				g[i + j].make_root();
			CkStartQD(CkCallbackResumeThread());
		}
	}
	void startMultiBFSPhase(const CkCallback & cb) {
		for (CmiUInt8 i = 0; i < nVertex; i += 4) { 
			for (int j = 0; j < 4; j++)
				g.make_root();
			CkStartQD(CkCallbackResumeThread());
		}
		CkStartQD(cb);
	}
	void startComputeCentralityPhase() {
		g.compute_centrality();
	}
	void startComputeCentralityPhase(const CkCallback & cb) {
		g.compute_centrality();
		CkStartQD(cb);
	}
	void verify() {
		g.verify();
	}
	void dump_edges() {
		g.dump_edges();
	}
	void dump_dist() {
		g.dump_dist();
	}
	void pup(PUP::er &p) {
		p | directed;
	}
};

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;

struct BetweenCentrEdge {
	CmiUInt8 v;

	BetweenCentrEdge() {}
	BetweenCentrEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BetweenCentrVertex : public CBase_BetweenCentrVertex {
private:
	std::vector<BetweenCentrEdge> adjlist;
	std::vector<unsigned int> distances;
	std::vector<unsigned int> shortest_paths;
	double centrality;
	unsigned int path_counter;

public:
  BetweenCentrVertex() : centrality(0), path_counter(0) {
		distances.assign(N, -1);
		shortest_paths.assign(N, 0);
  }

	void connectVertex(const BetweenCentrEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const BetweenCentrEdge & edge) {
		connectVertex(edge);
	}

  BetweenCentrVertex(CkMigrateMessage *msg) {}

	void make_root() {
		distances[thisIndex] = 0;
		shortest_paths[thisIndex] = 1;
		typedef typename std::vector<BetweenCentrEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			if (it->v != thisIndex)
				thisProxy[it->v].update(distances[thisIndex] + 1, shortest_paths[thisIndex], thisIndex);
		}
	}

  void update(unsigned int d, unsigned int paths, CmiUInt8 source) {
		if (distances[source] < d)
			return;

		if (distances[source] == d)
			shortest_paths[source]++;
		else {
			distances[source] = d;
			shortest_paths[source] = paths;
		}

		typedef typename std::vector<BetweenCentrEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(distances[source] + 1, shortest_paths[source], source);
		}
		//traceMemoryUsage();
  }

	void compute_centrality() {
		for (CmiUInt8 j = 0; j < N; j++) 
			if (j != thisIndex && distances[j] != -1)
				for (CmiUInt8 u = 0; u < N; u++) 
					if (u != thisIndex && distances[u] < distances[j])
						thisProxy[u].update_centrality(distances[u], distances[j], 
								shortest_paths[u], shortest_paths[j], j);
	}

	void update_centrality(unsigned int du, unsigned int dj, unsigned int paths_to_u, unsigned int paths_to_j, CmiUInt8 j) {
		//CkPrintf("%d: compute_centrality\n", thisIndex);
		if (dj == du + distances[j]) {
			centrality += 1.0 * paths_to_u * shortest_paths[j] / paths_to_j;
			path_counter++;
		}
	}

	void verify() {
		//if ((parent != -1) && (parent != thisIndex))
		//	thisProxy[parent].check(level);
	}
	void check(int level) {
		//CkAssert(this->level + 1 == level);
	}
	void dump_edges() { 
		typedef typename std::vector<BetweenCentrEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			CkPrintf("%lld,%lld\n", thisIndex, it->v);
		}
	}
	void dump_dist() { 
		typedef typename std::vector<unsigned int>::iterator Iterator; 
		CkPrintf("%d: (centrality %f, paths %lld), distances :", thisIndex, centrality, path_counter);
		//for (unsigned int i = 0; i < N; i++)
		//	CkPrintf("(%3d,%3d),", distances[i], shortest_paths[i]);
		CkPrintf("\n");
	}
};

typedef GraphLib::GraphGenerator<
	BetweenCentrGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphGeneratorType::GRAPH,
	GraphLib::TransportType::Tram
	> Generator;
#include "driver.C"
#include "charm_bc.def.h"
