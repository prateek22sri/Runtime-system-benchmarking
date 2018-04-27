#!/bin/sh

prefix=/N/u/pratsriv/BigRed2/hpx-4.1.0/hpx-install
exec_prefix=/N/u/pratsriv/BigRed2/hpx-4.1.0/hpx-install
libdir=${exec_prefix}/lib

LD_PRELOAD=${libdir}/libjemalloc.so.2
export LD_PRELOAD
exec "$@"
