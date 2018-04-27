bin_PROGRAMS = luleshparcels
luleshparcels_LINK = $(CXX) -o $@
luleshparcels_SOURCES 	= lulesh-comp.c lulesh-xfer.c lulesh-hpx.c \
                          lulesh-util.c sbn1.c sbn3.c posvel.c monoq.c

if ENABLE_TRACING
luleshparcels_SOURCES	+= trace.c
endif
