#!/bin/bash

echo "/run/beamOn 1000000" > tmp.mac

./E6ES -m tmp.mac -d 2
hadd -f normal.root result_t*

./E6ES -v -m tmp.mac -d 2
hadd -f vacuum.root result_t*

for ene in 3 5 7
do
    ./E6ES -e $ene -m tmp.mac -d 2
    hadd -f "$ene"GeV.root result_t*
done

echo "/run/beamOn 100000" > tmp.mac

./E6ES -m tmp.mac -d 2 -c 1
hadd -f collimator.root result_t*
