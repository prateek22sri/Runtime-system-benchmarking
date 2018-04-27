#!/usr/bin/env bash

#PBS -l nodes=2:ppn=16
#PBS -l walltime=03:00:00
#PBS -d /u/ldalessa/hpx-apps/lulesh/parcels
#PBS -k oe

source $HOME/.modules

trap "exit" INT

timeout="10m"

events="-e cycles"
events="$events -e stalled-cycles-frontend"
events="$events -e stalled-cycles-backend"
events="$events -e instructions"
events="$events -e branches"
events="$events -e branch-misses"
events="$events -e bus-cycles"
events="$events -e cache-misses"
events="$events -e dTLB-load-misses"
events="$events -e iTLB-load-misses"
events="$events -e cs"
events="$events -e migrations"
perf="perf stat $events --log-fd=1 -x ,"
mpirun="timeout $timeout mpirun --tag-output --mca mtl ^psm --mca btl_openib_if_include mlx4_0"

function dump_config {
    size=$(($n*$(($x**3))))
    echo lulesh
    echo size: $size
    echo np: $np
    echo n: $n
    echo x: $x
    echo i: 20
    echo pe: $pe
    echo net: photon
    echo build: lto
    echo link: static
    echo events: $events
}

function run1 {
    lulesh="./luleshparcels -n $n -x $x -i 20 --hpx-threads=$pe --hpx-opt-smp=0 --hpx-photon-ibdev=mlx4_0 --hpx-photon-numcq=2"
    run="$mpirun -np $np --map-by $topo:pe=$pe $perf $lulesh"
    echo $run
    $run
}

function runn { 
    lulesh="./luleshparcels -n $n -x $x -i 20 --hpx-threads=$pe --hpx-opt-smp=0 --hpx-photon-ibdev=mlx4_0 --hpx-photon-numcq=2"
    np1=$(($np - 1))
    run="$mpirun -np 1 --map-by $topo:pe=$pe perf stat $events --log-fd=1 -x , $lulesh : -np $np1 --map-by $topo:pe=$pe $lulesh"
    echo $run
    $run
}

function dump_finish {
    echo hselul $?
}

for i in 2,72 3,48 4,36 6,24 8,18 9,16 12,12 16,9 18,8 24,6 36,4 48,3
do
    n=$((${i%,*}**3))
    x=${i#*,}
    np=2
    topo="ppr:1:node"
    #for pe in {1..16}
    for pe in 16
    do
        dump_config
        run1
        dump_finish
    done
    np=4
    topo="ppr:1:numa"
    #for pe in {1..8}
    for pe in 8
    do
        dump_config
        run1
        dump_finish
    done
    np=8
    topo="ppr:2:numa"
    #for pe in {1..4}
    for pe in 4
    do
        dump_config
        run1
        dump_finish
    done
    np=16
    topo="ppr:4:numa"
    #for pe in {1..2}
    for pe in 2
    do
        dump_config
        run1
        dump_finish
    done
    # np=32
    # topo="ppr:8:numa"
    # for pe in 1
    # do
    #     dump_config
    #     run1
    #     dump_finish
    # done
done
