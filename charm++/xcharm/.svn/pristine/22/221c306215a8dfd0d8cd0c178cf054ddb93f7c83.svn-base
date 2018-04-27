include ../../../../config.mk
CHARMXI=../../../../bin/charmxi
CHARMC_COMPILE_FLAGS= -O3 -I./.tmp -I../.. -I../../../../include/graphrt  -I../../../../include/graphrt/graph500 \
											-I$(BOOST_ROOT)/include $(CFLAGS) -Wno-deprecated-declarations
CHARMC_LINK_FLAGS=-L./.tmp -L../../../../lib -lGraph \
									-L$(BOOST_ROOT)/lib  -lboost_system -lboost_timer -lboost_chrono #-module ampi
XCHARMC_COMPILE_FLAGS=-I. -I../../../include -I../../../include/ucharelib -I../../../include/NDMeshStreamer \
											$(CHARMC_COMPILE_FLAGS)
XCHARMC_LINK_FLAGS=-L. -L../../../lib -lmoduleuChareLib -lmoduleNDMeshStreamer \
									 -module completion -module ampi \
									 $(CHARMC_LINK_FLAGS)

BINARY = charm_sssp charm_multivertex_sssp \
				 tram_sssp_r1 tram_sssp_r16 tram_sssp_r32 tram_sssp_r64 \
				 tram_sssp_r256 tram_sssp_r512 tram_sssp_r1024 \
				 tram_multivertex_sssp
#				 tram_sssp tram_sssp_radix 
#				 ucharelib_sssp_async ucharelib_sssp_async_radix

RMAT_BINARY= charm_sssp-rmat tram_sssp-rmat charm_multivertex_sssp-rmat tram_multivertex_sssp-rmat 
SSCA_BINARY= charm_sssp-ssca tram_sssp-ssca charm_multivertex_sssp-ssca tram_multivertex_sssp-ssca 
RANDOM_BINARY= charm_sssp-random tram_sssp-random charm_multivertex_sssp-random tram_multivertex_sssp-random 

rmat: $(RMAT_BINARY) 

ssca: $(SSCA_BINARY) 

random: $(RANDOM_BINARY)

#all: dir $(BINARY)
all: dir rmat ssca random

charm_sssp-rmat: charm_sssp-rmat.o
charm_sssp-rmat.o: charm_sssp.C charm_sssp.decl.h charm_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_sssp-ssca: charm_sssp-ssca.o
charm_sssp-ssca.o: charm_sssp.C charm_sssp.decl.h charm_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_sssp-random: charm_sssp-random.o
charm_sssp-random.o: charm_sssp.C charm_sssp.decl.h charm_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_multivertex_sssp-rmat: charm_multivertex_sssp-rmat.o
charm_multivertex_sssp-rmat.o: charm_multivertex_sssp.C charm_multivertex_sssp.decl.h charm_multivertex_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_multivertex_sssp-ssca: charm_multivertex_sssp-ssca.o
charm_multivertex_sssp-ssca.o: charm_multivertex_sssp.C charm_multivertex_sssp.decl.h charm_multivertex_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

charm_multivertex_sssp-random: charm_multivertex_sssp-random.o
charm_multivertex_sssp-random.o: charm_multivertex_sssp.C charm_multivertex_sssp.decl.h charm_multivertex_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

#tram_sssp_r1: tram_sssp_r1.o
#tram_sssp_r1.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=1 
#
#tram_sssp_r16: tram_sssp_r16.o
#tram_sssp_r16.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=16 
#
#tram_sssp_r32: tram_sssp_r32.o
#tram_sssp_r32.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=32
#
#tram_sssp_r64: tram_sssp_r64.o
#tram_sssp_r64.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64
#
#tram_sssp_r256: tram_sssp_r256.o
#tram_sssp_r256.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64
#
#tram_sssp_r512: tram_sssp_r512.o
#tram_sssp_r512.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64
#
#tram_sssp_r1024: tram_sssp_r1024.o
#tram_sssp_r1024.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
#	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DRADIX=64

tram_sssp: tram_sssp.o
tram_sssp.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h

tram_sssp-rmat: tram_sssp-rmat.o
tram_sssp-rmat.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_sssp-ssca: tram_sssp-ssca.o
tram_sssp-ssca.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_sssp-random: tram_sssp-random.o
tram_sssp-random.o: tram_sssp.C tram_sssp.decl.h tram_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_multivertex_sssp-rmat: tram_multivertex_sssp-rmat.o
tram_multivertex_sssp-rmat.o: tram_multivertex_sssp.C tram_multivertex_sssp.decl.h tram_multivertex_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_multivertex_sssp-ssca: tram_multivertex_sssp-ssca.o
tram_multivertex_sssp-ssca.o: tram_multivertex_sssp.C tram_multivertex_sssp.decl.h tram_multivertex_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

tram_multivertex_sssp-random: tram_multivertex_sssp-random.o
tram_multivertex_sssp-random.o: tram_multivertex_sssp.C tram_multivertex_sssp.decl.h tram_multivertex_sssp.def.h
	$(CHARMC) $(CHARMC_COMPILE_FLAGS) -language charm++ -c -o ./.tmp/$@ $< -DGRAPH=RMAT

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
	rm -f $(RMAT_BINARY) $(SSCA_BINARY) $(RANDOM_BINARY)
	rm -f ./tmp/*.xi .tmp/*.decl.h .tmp/*.def.h .tmp/*.o charmrun 

clean-all:
	rm -rf .tmp

.PHONY: dir

dir:
	mkdir -p .tmp
