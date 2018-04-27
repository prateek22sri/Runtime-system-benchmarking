#!/bin/bash 
#PBS -k o
#PBS -l nodes=2:ppn=32,walltime=1:00:00
#PBS -N pbgl16_64
#PBS -q cpu
#PBS -m abe
#PBS -j oe
#PBS -M pratsriv@iu.edu

module swap PrgEnv-cray/5.2.82 PrgEnv-gnu
module load boost/1.65.0	
cd /N/u/pratsriv/BigRed2/HPCProject/Delta\ Stepping
aprun -n 64 a.out file16.gr 

