#!/bin/bash

#wget http://www.dis.uniroma1.it/challenge9/code/ch9-1.1.tar.gz
#tar xvfz ch9-1.1.tar.gz
#cd  ch9-1.1/
#cd ./scripts
#replace the url with exact one.
if [ $# -eq 0 ]; then
    ENV="/u/jsfiroz/HPX/hpx/apps/libPXGL/examples"
else
    ENV=$1
fi

echo $ENV
#make gen_gr #generate the real life graph
#make gen_ss #generate synthetic graph
#make run_ss $ENV
