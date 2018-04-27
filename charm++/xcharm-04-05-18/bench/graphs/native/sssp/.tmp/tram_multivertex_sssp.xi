mainmodule tram_multivertex_sssp {
 extern module GraphGenerator;
  readonly int K;
  readonly CmiUInt8 N;
  readonly CmiUInt8 M;
  readonly CProxy_TestDriver driverProxy;
  readonly CProxy_ArrayMeshStreamer<dtype, long long, SSSPMultiVertex,
                                    SimpleMeshRouter> aggregator;
  mainchare TestDriver {
    entry TestDriver (CkArgMsg *args);
    entry [threaded] void start();
    entry [threaded] void restart();
  entry void startVerificationPhase();
    entry [reductiontarget] void done(CmiUInt8 globalNumScannedEdges);
  entry void startGraphConstruction();
  entry void exit();
  };
 class SSSPEdge;
  array [1D] SSSPMultiVertex {
    entry SSSPMultiVertex();
  entry void connectVertex(std::pair<CmiUInt8, SSSPEdge> & edge);
  entry void make_root(const CmiUInt8 & root);
  entry void verify();
  entry void check(const CmiUInt8 & v, const double & weight);
  entry void getScannedVertexNum();
  entry void foo();
  };
  message MeshStreamerMessage<ArrayDataItem<std::pair<CmiUInt8, SSSPEdge>, long long> >;
  group ArrayMeshStreamer<std::pair<CmiUInt8, SSSPEdge>, long long, SSSPMultiVertex, SimpleMeshRouter>;
  group MeshStreamer<ArrayDataItem<std::pair<CmiUInt8, SSSPEdge>, long long>, SimpleMeshRouter>;
 group GraphLib__RMAT_Generator_Tram<SSSPGraph, Options,
  GraphLib::VertexMapping::MultiVertex,
  CProxy_ArrayMeshStreamer<std::pair<CmiUInt8, SSSPEdge>, long long, SSSPMultiVertex, SimpleMeshRouter> >;
 group GraphLib__RMAT_Generator_Charm<SSSPGraph, Options,
  GraphLib::VertexMapping::MultiVertex>;
 group GraphLib__SSCA_Generator_Tram<SSSPGraph, Options,
  GraphLib::VertexMapping::MultiVertex,
  CProxy_ArrayMeshStreamer<std::pair<CmiUInt8, SSSPEdge>, long long, SSSPMultiVertex, SimpleMeshRouter> >;
 group GraphLib__SSCA_Generator_Charm<SSSPGraph, Options,
  GraphLib::VertexMapping::MultiVertex>;
 group GraphLib__Random_Generator_Tram<SSSPGraph, Options,
  GraphLib::VertexMapping::MultiVertex,
  CProxy_ArrayMeshStreamer<std::pair<CmiUInt8, SSSPEdge>, long long, SSSPMultiVertex, SimpleMeshRouter> >;
 group GraphLib__Random_Generator_Charm<SSSPGraph, Options,
  GraphLib::VertexMapping::MultiVertex>;
};
