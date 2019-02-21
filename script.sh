#!/bin/bash


echo "/run/beamOn 1000000" > tmp.mac

for ene in 1 2 3 4 5 6 7 8 9 10
do
    ./E6ES -c 1 -d 3 -e $ene -m tmp.mac
    hadd -f Ene$ene.root result_t*
done
