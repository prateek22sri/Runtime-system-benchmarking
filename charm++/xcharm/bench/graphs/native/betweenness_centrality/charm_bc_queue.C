#include <GraphLib.h>
#include <common.h>

class BetweenCentrVertex;
class BetweenCentrEdge;
class BetweenCentrGraph;

#include "charm_bc_queue.decl.h"

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_MultiBFSDriver multiBFSDriver;

class MultiBFSDriver : public CBase_MultiBFSDriver {
	public:
		MultiBFSDriver (CProxy_BetweenCentrVertex & g) : g(g), empty(true) {} 
		void run() {
			//CkPrintf("MultiBFSDriver::run\n");
			CkWaitQD();

			while (!isEmpty()) {
				while (!localFront.empty()) {
					std::pair<CmiUInt8,CmiUInt8> el = localFront.front();
					localFront.pop_front();
					g[el.first].resume(el.second);
				}
				CkWaitQD();
			}
			//CkPrintf("MultiBFSDriver::run finished\n");
			driverProxy.set_fin(true);
		}
		void addVertexToFront(CmiUInt8 vertex, CmiUInt8 source) {
			//CkPrintf("<%lld, %lld> added to queue\n", vertex, source);
			localFront.push_back(std::make_pair(vertex, source));
		}
		bool isEmpty() {
			thisProxy.test();
			CkWaitQD();
			return empty;
		} 
		void test() {
			CmiUInt8 c = (localFront.empty() ? 1 : 0);
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long, 
					CkCallback(CkReductionTarget(MultiBFSDriver, check), thisProxy));
		}
		void check(CmiUInt8 tot) {
			empty = tot == CkNumPes() ? true : false;
		}
	private:
		volatile bool empty;
		std::list<std::pair<CmiUInt8,CmiUInt8>> localFront;
		CProxy_BetweenCentrVertex g;
};

class BetweenCentrGraph : public GraphLib::Graph<
	BetweenCentrVertex,
	BetweenCentrEdge,
	CProxy_BetweenCentrVertex,
	GraphLib::TransportType::Charm> {
public:
	bool directed;
	CmiUInt8 nVertex;
	volatile bool fin;
public:
	BetweenCentrGraph(bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BetweenCentrVertex, 
				BetweenCentrEdge,
				CProxy_BetweenCentrVertex, 
				GraphLib::TransportType::Charm >() {
	}
	BetweenCentrGraph(CmiUInt8 nVertex, bool directed = true) : 
		nVertex(nVertex),
	  directed(directed),	
		GraphLib::Graph<
				BetweenCentrVertex, 
				BetweenCentrEdge,
				CProxy_BetweenCentrVertex, 
				GraphLib::TransportType::Charm >(nVertex)	{

		CkPrintf("BetweenCentrGraph::create multiBFSDriver\n");
		multiBFSDriver = CProxy_MultiBFSDriver::ckNew(g); 
	}
	BetweenCentrGraph(const CProxy_BetweenCentrVertex & g, bool directed = true) : 
		directed(directed),
		GraphLib::Graph<
				BetweenCentrVertex, 
				BetweenCentrEdge,
				CProxy_BetweenCentrVertex, 
				GraphLib::TransportType::Charm >(g)	{

		multiBFSDriver = CProxy_MultiBFSDriver::ckNew(g); 
	}
	void startMultiBFSPhase() {
		//g.make_root();
		int l = 16;
		for (CmiUInt8 i = 0; i < nVertex; i += l) { 
			CkPrintf("multi-bfs: %d/%d\n",i,nVertex);
			for (int j = 0; j < l; j++)
				g[i + j].make_root();
			
			multiBFSDriver.run();

			fin = false;
			while(!fin)
				CkStartQD(CkCallbackResumeThread());
		}
	}

	void set_fin(bool val) {
		fin = val;
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
		p | g;
	}
};

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
	std::list<CmiUInt8> sources;
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
			//sources.push_back(source);
			//CkStartQD(CkIndex_BetweenCentrVertex::resume(), &thishandle);
			multiBFSDriver.ckLocalBranch()->addVertexToFront(thisIndex, source);
		}
  }

	void resume(CmiUInt8 source) {
		//CkAssert(!sources.empty());
		//CmiUInt8 source = sources.front();
		//sources.pop_front();
		typedef typename std::vector<BetweenCentrEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(distances[source] + 1, shortest_paths[source], source);
		}
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
		/*for (unsigned int i = 0; i < N; i++)
			CkPrintf("(%3d,%3d),", distances[i], shortest_paths[i]);*/
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

#define BC_QUEUE
#include "driver.C"
#include "charm_bc_queue.def.h"
