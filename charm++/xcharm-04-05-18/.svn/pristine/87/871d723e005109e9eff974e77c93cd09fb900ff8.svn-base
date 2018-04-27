#include <GraphLib.h>
#include <common.h>

class BFSVertex;
class BFSEdge;
class BFSGraph;

#include "charm_bfs_queue.decl.h"

#define CHARM_BFS_QUEUE

CmiUInt8 N, M;
int K = 16;
CProxy_TestDriver driverProxy;
CProxy_BFSQueue Q;

class BFSQueue : public CBase_BFSQueue {
	public:
		BFSQueue(CProxy_BFSVertex & g) : g(g) {}
		void addVertex(CmiUInt8 v) { local_queue.push_back(v); }
		void run() {
			CkWaitQD();
			while (!isEmpty()) {
				while (!local_queue.empty()) {
					g[local_queue.front()].resume();
					local_queue.pop_front();
				}
				CkWaitQD();
			}
			driverProxy.set_fin();
		}
		bool isEmpty() {
			thisProxy.test();
			CkWaitQD();
			return empty;
		}
		void test() {
			CmiUInt8 c = (local_queue.empty() ? 1 : 0);
			contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
					CkCallback(CkReductionTarget(BFSQueue, check), thisProxy));
		}
		void check(CmiUInt8 tot) {
			empty = tot == CkNumPes() ? true : false;
		}
	private:
		std::list<CmiUInt8> local_queue;
		CProxy_BFSVertex g;
		volatile bool empty;
};

class BFSGraph : public GraphLib::Graph<
	BFSVertex,
	BFSEdge,
	CProxy_BFSVertex,
	GraphLib::TransportType::Charm> {
public:
	bool directed;
	volatile bool fin;
public:
	BFSGraph(bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >()	
		{}
	BFSGraph(CmiUInt8 nVertex, bool directed = true) : 
	  directed(directed),	
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(nVertex)	{
		Q = CProxy_BFSQueue::ckNew(g);
	}
	BFSGraph(const CProxy_BFSVertex & g, bool directed = true) : 
		directed(directed),
		GraphLib::Graph<
				BFSVertex, 
				BFSEdge,
				CProxy_BFSVertex, 
				GraphLib::TransportType::Charm >(g) {
		Q = CProxy_BFSQueue::ckNew(g);
	}
	void start(CmiUInt8 root) {
		g[root].make_root();
		Q.run();
		fin = false;
		while (!fin) CkWaitQD();
	}
	void start(CmiUInt8 root, const CkCallback & cb) {
		g[root].make_root();
		CkStartQD(cb);
	}
	void getScannedVertexNum() {
		g.getScannedVertexNum();
	}
	void verify() {
		g.verify();
	}
	void dump_edges() {
		g.dump_edges();
	}
	void set_fin() {
		fin = true;
	}
	void pup(PUP::er &p) {
		p | directed;
		p | g;
	}
};

struct BFSEdge {
	CmiUInt8 v;

	BFSEdge() {}
	BFSEdge(CmiUInt8 v, double w) : v(v) {}
	void pup(PUP::er &p) { 
		p | v; 
	}
};

class BFSVertex : public CBase_BFSVertex {
private:
	std::vector<BFSEdge> adjlist;
	unsigned int level;
	CmiUInt8 parent;

public:
  BFSVertex() : level(-1), parent(-1) {
    // Contribute to a reduction to signal the end of the setup phase
    //contribute(CkCallback(CkReductionTarget(TestDriver, start), driverProxy));
  }

	void connectVertex(const BFSEdge & edge) {
		adjlist.push_back(edge);
	}

	void process(const BFSEdge & edge) {
		connectVertex(edge);
	}

  BFSVertex(CkMigrateMessage *msg) {}

	void make_root() {
		//CkPrintf("%d: updated\n", thisIndex);
		this->level = 0;
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			if (it->v != thisIndex)
				thisProxy[it->v].update(this->level + 1, thisIndex);
		}
	}

  void update(unsigned int level, CmiUInt8 parent) {
		if (this->level < level)
			return;
		//CkPrintf("%d: %d\n",CkMyPe(), thisIndex);
		//CkPrintf("%d: updated level(%d->%d), parent(%lld->%lld)\n", 
		//		thisIndex, this->level, level, this->parent, parent);
		if (this->level == -1) 
			Q.ckLocalBranch()->addVertex(thisIndex);

		this->level = level;
		this->parent = parent;
  }

	void resume() {
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			thisProxy[it->v].update(this->level + 1, thisIndex);
		}
	}

	void getScannedVertexNum() {
		CmiUInt8 c = (parent == -1 ? 0 : 1);
		contribute(sizeof(CmiUInt8), &c, CkReduction::sum_long,
				CkCallback(CkReductionTarget(TestDriver, done), driverProxy));
	}

	void verify() {
		if ((parent != -1) && (parent != thisIndex))
			thisProxy[parent].check(level);
	}
	void check(int level) {
		CkAssert(this->level + 1 == level);
	}
	void dump_edges() {
		typedef typename std::vector<BFSEdge>::iterator Iterator; 
		for (Iterator it = adjlist.begin(); it != adjlist.end(); it++) {
			CkPrintf("%lld,%lld\n", thisIndex, it->v);
		}
	}
};

typedef GraphLib::GraphGenerator<
	BFSGraph, 
	Options, 
	GraphLib::VertexMapping::SingleVertex,
	GraphLib::GraphGeneratorType::GRAPH,
	GraphLib::TransportType::Tram
	> Generator;
#include "driver.C"
#include "charm_bfs_queue.def.h"
