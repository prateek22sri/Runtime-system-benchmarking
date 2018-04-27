#!/bin/sh

# Simple weak scaling study with eam potential and 32000 atoms per task
mpirun -np 1  ../bin/CoMD-hpx -i 1 -j 1 -k 1 -x 20 -y 20 -z 20
mpirun -np 2  ../bin/CoMD-hpx -i 2 -j 1 -k 1 -x 40 -y 20 -z 20
mpirun -np 4  ../bin/CoMD-hpx -i 2 -j 2 -k 1 -x 40 -y 40 -z 40
mpirun -np 8  ../bin/CoMD-hpx -i 2 -j 2 -k 2 -x 40 -y 40 -z 40
mpirun -np 16 ../bin/CoMD-hpx -i 4 -j 2 -k 2 -x 80 -y 40 -z 40
