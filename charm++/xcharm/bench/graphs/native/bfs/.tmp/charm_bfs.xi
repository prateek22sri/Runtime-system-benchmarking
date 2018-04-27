mainmodule charm_bfs {
 extern module GraphGenerator;
  readonly int K;
  readonly CmiUInt8 N;
  readonly CmiUInt8 M;
  readonly CProxy_TestDriver driverProxy;
  mainchare TestDriver {
    entry TestDriver (CkArgMsg *args);
    entry void start();
    entry void restart();
  entry void startVerificationPhase();
    entry [reductiontarget] void done(CmiUInt8 globalNumScannedEdges);
  entry void startGraphConstruction();
  entry void exit();
  };
 class BFSEdge;
  array [1D] BFSVertex {
    entry BFSVertex();
  entry void connectVertex(const BFSEdge & edge);
  entry void make_root();
  entry void update(int level, CmiUInt8 parent);
  entry void getScannedVertexNum();
  entry void verify();
  entry void check(int level);
  entry void dump_edges();
  };
  message MeshStreamerMessage<ArrayDataItem<BFSEdge, long long> >;
  group ArrayMeshStreamer<BFSEdge, long long, BFSVertex, SimpleMeshRouter>;
  group MeshStreamer<ArrayDataItem<BFSEdge, long long>, SimpleMeshRouter>;
 group GraphLib__RMAT_Generator_Charm<BFSGraph, Options,
  GraphLib::VertexMapping::SingleVertex>;
 group GraphLib__RMAT_Generator_Tram<BFSGraph, Options,
  GraphLib::VertexMapping::SingleVertex,
  CProxy_ArrayMeshStreamer<BFSEdge, long long, BFSVertex, SimpleMeshRouter> >;
 group GraphLib__SSCA_Generator_Charm<BFSGraph, Options,
  GraphLib::VertexMapping::SingleVertex>;
 group GraphLib__SSCA_Generator_Tram<BFSGraph, Options,
  GraphLib::VertexMapping::SingleVertex,
  CProxy_ArrayMeshStreamer<BFSEdge, long long, BFSVertex, SimpleMeshRouter> >;
 group GraphLib__Random_Generator_Charm<BFSGraph, Options,
  GraphLib::VertexMapping::SingleVertex>;
 group GraphLib__Random_Generator_Tram<BFSGraph, Options,
  GraphLib::VertexMapping::SingleVertex,
  CProxy_ArrayMeshStreamer<BFSEdge, long long, BFSVertex, SimpleMeshRouter> >;
};
