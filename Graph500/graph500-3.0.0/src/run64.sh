#!/bin/bash 
#PBS -k o 
#PBS -l nodes=2:ppn=32,walltime=1:00:00
#PBS -M pratsriv@iu.edu
#PBS -m abe 
#PBS -N graph500_64
#PBS -j oe 

cd ~/graph500-3.0.0/src/

TMPFILE=scale16 REUSEFILE=1 aprun -n 64 ./graph500_reference_bfs_sssp 16
