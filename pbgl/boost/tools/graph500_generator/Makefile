BOOST_INCLUDE=/N/soft/cle5/boost/1.65.0/include/boost
CXX = CC -I$(BOOST_INCLUDE)
CC = cc

all: libgraph500_generator.a

GENERATOR_SOURCES =  graph500-3.0.0/generator/graph_generator.c \
		graph500-3.0.0/generator/splittable_mrg.c graph500-3.0.0/generator/utils.c
#graph500-3.0.0/generator/make_graph.c 

libgraph500_generator.a: $(GENERATOR_SOURCES:.c=.o) 
	ar q $@ $^

%.cpp: %.o
	$(CXX) -c $< -o $@

%.o: %.c
	$(CC) -DSSSP -c $< -o $@ 

clean:
	rm -f $(GENERATOR_SOURCES:.c=.o) libgraph500_generator.a
