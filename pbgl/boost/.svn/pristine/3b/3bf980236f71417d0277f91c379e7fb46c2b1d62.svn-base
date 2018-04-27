CXX=/home/frolo/local/openmpi-2.0.0-gcc-4.9/bin/mpicxx -g -msse
CXX_INCLUDE= -I/home/frolo/pbgl2 -I/home/frolo/pbgl2/include \
						 -I/home/frolo/local/libnbc/include -I/home/frolo/local/include -I../tools/graph500 \
						 -I/home/frolo/local/am++/include -I/home/frolo/wdir/pbgl2 \
 						 -I/home/frolo/local/boost_1_54_0/include \
						 -DPROFILE_ENABLED -DDEBUG -Wno-deprecated-declarations
#CXX_LIBS=-L/home/frolo/local/boost_1_58_0/lib -lboost_graph -lboost_graph_parallel -lboost_mpi \
#	-lboost_serialization -lboost_system -lboost_timer -lboost_chrono -lboost_program_options
CXX_LIBS=-L/home/frolo/local/boost_1_54_0/lib -L/home/frolo/local/lib/ -L/home/frolo/local/am++/lib \
				 -lampp -lboost_system -lboost_thread -lboost_chrono -lboost_program_options -lboost_timer

TESTS= pbgl2_bfs pbgl2_async-bfs pbgl2_sssp pbgl2_parallel_search \
			 pbgl2_pagerank pbgl2_sssp_delta pbgl2_cc

all: $(TESTS) 

#%.o: %.cpp
#	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

stdlib_random.o: stdlib_random.cpp 
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_bfs: pbgl2_bfs.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS) 

pbgl2_bfs.o: pbgl2_bfs.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_sssp: pbgl2_sssp.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS) 

pbgl2_sssp.o: pbgl2_sssp.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_async-bfs: pbgl2_async-bfs.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS)

pbgl2_async-bfs.o: pbgl2_async-bfs.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_parallel_search: pbgl2_parallel_search.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS)

pbgl2_parallel_search.o: pbgl2_parallel_search.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_pagerank: pbgl2_pagerank.o
	$(CXX) -o $@ $< $(CXX_LIBS)

pbgl2_pagerank.o: pbgl2_pagerank.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_sssp_delta: pbgl2_sssp_delta.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS) 

pbgl2_sssp_delta.o: pbgl2_sssp_delta.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

pbgl2_cc: pbgl2_cc.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS) 

pbgl2_cc.o: pbgl2_cc.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

distributed_connected_components_test: distributed_connected_components_test.o stdlib_random.o
	$(CXX) -o $@ $^ $(CXX_LIBS) 

distributed_connected_components_test.o: distributed_connected_components_test.cpp
	$(CXX) -c -o $@ $< $(CXX_INCLUDE)

clean:
	rm -f *.o $(TESTS)
