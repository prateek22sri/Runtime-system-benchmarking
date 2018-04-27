CC      = icc # provide a CILK enabled compiler
CCFLAGS = -Wall -O3 -g -std=gnu99 -D$(CASE) -D$(INTEG) $(OUTPUT)
INCLUDE = 
LIBS    = 
LINKER  = $(CC)
LKFLAGS = -lm #  Add these if you want output/parameter file support: -lsilo -lbbhutil
EXEC    = hawaii
CASE    = RMHD
OUTPUT  = #-DSILO -DRNPL  Add these if you want output/parameter file support
INTEG   = RK4 #FORWARD_EULER

SRCC    = $(shell ls *.c)
OBJC    = $(SRCC:.c=.o)

CILK = 1

ifeq ($(CILK),1)
INCLUDE+= $(shell pkg-config --cflags hpx) 
LIBS   += $(shell pkg-config --libs hpx)
CCFLAGS+= -DCILK=1
endif

all: $(EXEC)

$(EXEC): $(OBJC)
	$(LINKER) -o $(EXEC) $(OBJC) $(LIBS) $(LKFLAGS)

%.o:%.c
	$(CC) $(CCFLAGS) -c $< -o $@ $(INCLUDE)

clean:
	rm -f $(EXEC) *.o *~ core *.sdf



