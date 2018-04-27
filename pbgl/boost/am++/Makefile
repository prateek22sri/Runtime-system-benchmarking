#CXX= mpic++ -g  -msse
PREP=/home/frolo/local/bin/scorep --user
CXX=$(PREP) /home/frolo/local/openmpi-2.0.0-gcc-4.9/bin/mpicxx -g -msse 
CXX_INCLUDE= -I/home/frolo/local/libnbc/include -I/home/frolo/local/include -I../tools/graph500 \
						 -I/home/frolo/local/am++/include \
						 -I/home/frolo/local/include \
						 -I/home/frolo/local/boost_1_54_0/include \
						 -DPROFILE_ENABLED -DDEBUG 
#CXX_LIBS=-L/home/frolo/local/boost_1_58_0/lib -lboost_graph -lboost_graph_parallel -lboost_mpi \
#	-lboost_serialization -lboost_system -lboost_timer -lboost_chrono -lboost_program_options
CXX_LIBS=-L/home/frolo/local/boost_1_54_0/lib -L/home/frolo/local/lib/ -L/home/frolo/local/am++/lib \
				 -lampp -lboost_system -lboost_thread -lboost_chrono -lboost_program_options -lboost_timer

all: random_access

random_access: random_access.o
	$(CXX) -o $@ $< $(CXX_LIBS) #GraphHPC-1.0/graph_tools.o

random_access.o: random_access.cpp
	$(CXX) -c -o $@ $< -std=c++11 $(CXX_INCLUDE)

clean:
	rm -f random_access *.o 
