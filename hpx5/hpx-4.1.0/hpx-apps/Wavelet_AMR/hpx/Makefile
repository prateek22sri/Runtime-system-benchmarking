CC      = mpicc
CCFLAGS = -Wall -O3 -g -std=gnu99 $(OUTPUT)
LINKER  = $(CC)
LKFLAGS = -lm #-lsilo -lbbhutil  Add these libraries if you want I/O
EXEC    = hawaii
OUTPUT  = #-DSILO -DRNPL  Add these if you want I/O
SRCC    = $(shell ls *.c)
OBJC    = $(SRCC:.c=.o)

HPX=1

ifeq ($(HPX),1)
HPXINCLUDE+= $(shell pkg-config --cflags hpx)
LIBS   += $(shell pkg-config --libs hpx)
CCFLAGS+= -DHPX=1
endif

all: $(EXEC)

$(EXEC): $(OBJC)
	$(LINKER) -o $(EXEC) $(OBJC) $(LIBS) $(LKFLAGS)

%.o:%.c
	$(CC) $(CCFLAGS) -c $< -o $@ $(HPXINCLUDE)

clean:
	rm -f $(EXEC) *.o *~ core *.sdf *.hawaii *.pdb
