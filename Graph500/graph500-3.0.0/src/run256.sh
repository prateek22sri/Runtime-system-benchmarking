#!/bin/bash 
#PBS -k o 
#PBS -l nodes=8:ppn=32,walltime=1:00:00
#PBS -M pratsriv@iu.edu
#PBS -m abe 
#PBS -N graph500_256 
#PBS -j oe 

cd ~/graph500-3.0.0/src/

TMPFILE=scale25 REUSEFILE=1 aprun -n 256 ./graph500_reference_bfs_sssp 25
