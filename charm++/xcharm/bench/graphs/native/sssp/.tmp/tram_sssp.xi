mainmodule tram_sssp {
 extern module GraphGenerator;
  readonly CProxy_TestDriver driverProxy;
  readonly CProxy_ArrayMeshStreamer<dtype, long long, SSSPVertex,
                                    SimpleMeshRouter> aggregator;
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
  entry [threaded] void process(const dtype & m);
  entry [threaded] void update(const dtype & m);
  entry void getScannedVertexNum();
  entry void verify();
  entry void check(const double & w);
  entry void print();
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
