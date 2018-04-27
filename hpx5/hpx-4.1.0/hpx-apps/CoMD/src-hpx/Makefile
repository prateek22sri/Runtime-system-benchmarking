# Makefile for src-hpx
SHELL = /bin/bash

### Set your desired C compiler and any necessary flags.  Note that CoMD
### uses some c99 features.  You can also set flags for optimization and
### specify paths to include files that the compiler can't find on its
### own.  If you need any -L or -l switches to get C standard libraries
### (such as -lm for the math library) put them in LKFLAGS.
CC = gcc
CCFLAGS = -O3 -g -std=gnu99
LKFLAGS = -lm
INCLUDE =
LIBS =

# libraries for HPX
INCLUDE += $(shell pkg-config --cflags hpx)
LIBS  += $(shell pkg-config --libs hpx)

#########################################
### Below here, it is pitch black.  
### You are likely to be eaten by a grue.
##########################################

# clear all suffixes
.SUFFIXES:
# list only those that we use 
.SUFFIXES: .c .o

.PHONY: DEFAULT clean distclean depend

BIN_DIR=../bin

# Set executable name and add includes & libraries, if needed.
CoMD_VARIANT = CoMD-hpx
CoMD_EXE = ${BIN_DIR}/${CoMD_VARIANT}

SOURCES=$(shell ls *.c)
OBJECTS=$(SOURCES:.c=.o)

DEFAULT: ${CoMD_EXE}

%.o: %.c
	${CC} ${CCFLAGS} -c $< -o $@ ${INCLUDE}

${CoMD_EXE}: ${BIN_DIR} CoMD_info.h ${OBJECTS} 
	${CC} -o ${CoMD_EXE} ${OBJECTS} ${LIBS} ${LKFLAGS}

CoMD_info.h: Makefile
	./generate_info_header ${CoMD_VARIANT} "$(CC)" "$(CCFLAGS)" "$(LKFLAGS)"

${BIN_DIR}:
	@if [ ! -d ${BIN_DIR} ]; then mkdir -p ${BIN_DIR} ; fi

clean:
	rm -f *.o CoMD_info.h .depend

distclean: clean
	rm -f ${CoMD_EXE} .depend.bak
	rm -rf html latex

.depend: $(SOURCES)
	@touch .depend
	@$(MAKE) --no-print-directory depend

depend:
	@echo "Rebuilding dependencies..."
	@$(MAKE) CoMD_info.h
	@makedepend -f .depend -Y. --$(CFLAGS)-- $(SOURCES) 2> /dev/null

-include .depend
