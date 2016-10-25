#!/bin/bash

nEvents=1000000

#for ene in 1 2 8 9
for((i=1;i<=9;i++))
do
    echo "/run/beamOn $nEvents" > tmp.mac
    
    ./E6ES -m tmp.mac -e $i -z
    hadd -f "$i"GeV.root result_t*
    
    rm -f tmp.mac
done

rm -f result*
