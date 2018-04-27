mainmodule charm_sssp {
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
 class SSSPEdge;
  array [1D] SSSPVertex {
    entry SSSPVertex();
  entry void connectVertex(const SSSPEdge & edge);
  entry void make_root();
  entry void update(const CmiInt8 & v, const double & w);
  entry void verify();
  entry void getScannedVertexNum();
  entry void check(const double & w);
  entry void print();
  entry void dump_edges();
  };
  message MeshStreamerMessage<ArrayDataItem<SSSPEdge, long long> >;
  group ArrayMeshStreamer<SSSPEdge, long long, SSSPVertex, SimpleMeshRouter>;
  group MeshStreamer<ArrayDataItem<SSSPEdge, long long>, SimpleMeshRouter>;
 group GraphLib__RMAT_Generator_Charm<SSSPGraph, Options,
  GraphLib::VertexMapping::SingleVertex>;
 group GraphLib__RMAT_Generator_Tram<SSSPGraph, Options,
  GraphLib::VertexMapping::SingleVertex,
  CProxy_ArrayMeshStreamer<SSSPEdge, long long, SSSPVertex, SimpleMeshRouter> >;
 group GraphLib__SSCA_Generator_Charm<SSSPGraph, Options,
  GraphLib::VertexMapping::SingleVertex>;
 group GraphLib__SSCA_Generator_Tram<SSSPGraph, Options,
  GraphLib::VertexMapping::SingleVertex,
  CProxy_ArrayMeshStreamer<SSSPEdge, long long, SSSPVertex, SimpleMeshRouter> >;
 group GraphLib__Random_Generator_Charm<SSSPGraph, Options,
  GraphLib::VertexMapping::SingleVertex>;
 group GraphLib__Random_Generator_Tram<SSSPGraph, Options,
  GraphLib::VertexMapping::SingleVertex,
  CProxy_ArrayMeshStreamer<SSSPEdge, long long, SSSPVertex, SimpleMeshRouter> >;
};
