include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -g -O3 -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS) -Wno-deprecated-declarations
CHARMC_LINK_FLAGS= -L./.tmp -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono #-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

RMAT_BINARY= charm_bc-rmat charm_bc_queue-rmat
SSCA_BINARY= charm_bc-ssca  charm_bc_queue-ssca
RANDOM_BINARY= charm_bc-random  charm_bc_queue-random
RING_BINARY = charm_bc-ring  charm_bc_queue-ring

BINARY = $(RMAT_BINARY) $(SSCA_BINARY) $(RANDOM_BINARY) $(RING_BINARY)

rmat: $(RMAT_BINARY) 

ssca: $(SSCA_BINARY) 

random: $(RANDOM_BINARY)

all: dir $(BINARY)

charm_bc-rmat: charm_bc-rmat.o
charm_bc-rmat.o: charm_bc.C charm_bc.decl.h charm_bc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_bc-ssca: charm_bc-ssca.o
charm_bc-ssca.o: charm_bc.C charm_bc.decl.h charm_bc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=SSCA

charm_bc-random: charm_bc-random.o
charm_bc-random.o: charm_bc.C charm_bc.decl.h charm_bc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Random

charm_bc-ring: charm_bc-ring.o
charm_bc-ring.o: charm_bc.C charm_bc.decl.h charm_bc.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Ring

charm_bc_queue-rmat: charm_bc_queue-rmat.o
charm_bc_queue-rmat.o: charm_bc_queue.C charm_bc_queue.decl.h charm_bc_queue.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_bc_queue-ssca: charm_bc_queue-ssca.o
charm_bc_queue-ssca.o: charm_bc_queue.C charm_bc_queue.decl.h charm_bc_queue.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=SSCA

charm_bc_queue-random: charm_bc_queue-random.o
charm_bc_queue-random.o: charm_bc_queue.C charm_bc_queue.decl.h charm_bc_queue.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Random

charm_bc_queue-ring: charm_bc_queue-ring.o
charm_bc_queue-ring.o: charm_bc_queue.C charm_bc_queue.decl.h charm_bc_queue.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=Ring

%: %.o 
	$(CHARMC) $(CHARMC_LINK_FLAGS) -language charm++ -o $@ .tmp/$< -module NDMeshStreamer

%.o: %.C %.decl.h %.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< 

%.xi : %.ci
	gcc -x c++ -E -P $< > $@

%.def.h %.decl.h: %.xi
	$(CHARMXI) $<
	sed -i 's/CkIndex_GraphLib__/GraphLib::CkIndex_/g' $(basename $<).decl.h $(basename $<).def.h
	sed -i 's/GraphLib__/GraphLib::/g' $(basename $<).decl.h $(basename $<).def.h
	mv *.xi .tmp && mv *.decl.h .tmp && mv *.def.h .tmp

clean: 
	rm -f $(BINARY) 
	rm -f .tmp/*.xi .tmp/*.decl.h .tmp/*.def.h .tmp/*.o charmrun 

clean-all:
	rm -rf .tmp

.PHONY: dir

dir:
	mkdir -p .tmp
