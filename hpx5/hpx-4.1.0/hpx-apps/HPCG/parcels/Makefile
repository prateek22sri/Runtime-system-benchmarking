CC      = clang
CXX     = clang++
# use -DHPCG_USE_MPI to activate MPI reduction support (needs MPI headers).
CCFLAGS = -Wall -O3 -ffast-math $(OUTPUT)
CXXFLAGS = -Wall -O3 -ffast-math
INCLUDE =
LIBS    =
LINKER  = $(CXX)
LKFLAGS = -lm
EXEC    = hpcg
OUTPUT  =
SRCC    = $(shell ls *.c)
SRCXX   = $(shell ls *.cpp)
OBJC    = $(SRCC:.c=.o)
OBJCXX  = $(SRCXX:.cpp=.o)

INCLUDE+= $(shell pkg-config --cflags hpx)
LIBS   += $(shell pkg-config --libs hpx)

all: $(EXEC)

$(EXEC): $(OBJC) $(OBJCXX)
	$(LINKER) -o $(EXEC) $(OBJC) $(OBJCXX) $(LIBS) $(LKFLAGS)

%.o:%.c
	$(CXX) $(CCFLAGS) -c $< -o $@ $(INCLUDE)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(EXEC) *.o *~ core *.txt *.yaml
