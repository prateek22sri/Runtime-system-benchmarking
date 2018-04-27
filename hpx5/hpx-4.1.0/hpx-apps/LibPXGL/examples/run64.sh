#!/bin/bash 
#PBS -k o
#PBS -l nodes=2:ppn=32,walltime=01:00:00 
#PBS -q cpu 
#PBS -M pratsriv@iu.edu
#PBS -V 
#PBS -m abe 
#PBS -N hpx5_16_64 
#PBS -j oe 

cd /N/u/pratsriv/BigRed2/hpx-4.1.0/hpx-apps/LibPXGL/examples/

aprun -b -n 64 -N 32 ./sssp -g 16 -b --hpx-heapsize=$((1024 * 1024 * 1024 * 80 / 32)) --hpx-coalescing-buffersize=30000  --hpx-thread-affinity=none
