class TestDriver : public CBase_TestDriver {
private:
  double starttime, totaltime;
	Options opts;

	BetweenCentrGraph *graph;
	Generator *generator;

public:
  TestDriver(CkArgMsg* args) {
		parseCommandOptions(args->argc, args->argv, opts);
    N = opts.N;
		M = opts.M;
#ifdef RADIX_ENABLED
		R = opts.R;
#endif

    driverProxy = thishandle;

    // Create the chares storing vertices
    // Create graph
    graph = new BetweenCentrGraph(N, false);
		// create graph generator
		generator = new Generator(*graph, opts);

    starttime = CkWallTimer();
		CkStartQD(CkIndex_TestDriver::startGraphConstruction(), &thishandle);
    delete args;
  }

  void startGraphConstruction() {
		CkPrintf("BetweenCentr running...\n");
		CkPrintf("\tnumber of mpi processes is %d\n", CkNumPes());
		CkPrintf("\tgraph (s=%d, k=%d), scaling: %s\n", opts.scale, opts.K, (opts.strongscale) ? "strong" : "weak");
		CkPrintf("Start graph construction:........\n");
    starttime = CkWallTimer();
		generator->generate(CkCallback(CkIndex_TestDriver::startMultiBFSPhase(), thishandle));
		//CkStartQD(CkIndex_TestDriver::start(), &thishandle);
	}


  void startMultiBFSPhase() {
    double update_walltime = CkWallTimer() - starttime;
		CkPrintf("Initializtion completed:\n");
    CkPrintf("CPU time used = %.6f seconds\n", update_walltime);
    CkPrintf("start multi-bfs...\n");
    starttime = CkWallTimer();
		totaltime = 0;
		graph->startMultiBFSPhase();
		CkStartQD(CkIndex_TestDriver::startComputeCentralityPhase(), &thishandle);
  }

	void startComputeCentralityPhase() {
    CkPrintf("start computation of centralities...\n");
		graph->startComputeCentralityPhase();
		CkStartQD(CkIndex_TestDriver::startVerificationPhase(), &thishandle);
	}

  void restart() {
		CkAbort("never call restart");
  }

  void startVerificationPhase() {
		//totaltime += CkWallTimer() - starttime;
		done();
  }

  void done() {
		double totaltime = CkWallTimer() - starttime;
		CkPrintf("[Final] CPU time used = %.6f seconds\n", totaltime);
		if (opts.verify) { 
			CkPrintf("Start verification...\n");
			graph->verify();
		}
		graph->dump_dist();
		//graph->dump_edges();
		CkStartQD(CkIndex_TestDriver::exit(), &thishandle);
  }

	void exit() {
		CkPrintf("Done.\n");
		CkExit();
	}
#ifdef BC_QUEUE
	void set_fin(bool val) {graph->set_fin(val);}
#endif
};

