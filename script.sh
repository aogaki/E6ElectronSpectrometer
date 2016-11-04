#!/bin/bash


echo "/run/beamOn 100000" > tmp.mac

for ene in 1 2 3 4 5 6 7 8 9
do
    ./E6ES -m tmp.mac -e $ene -d 1 -c 1
    hadd -f "$ene"GeV_HA.root result_t*

    ./E6ES -m tmp.mac -e $ene -d 1 -c 2
    hadd -f "$ene"GeV_HV.root result_t*

    ./E6ES -m tmp.mac -e $ene -d 2 -c 1
    hadd -f "$ene"GeV_VA.root result_t*

    ./E6ES -m tmp.mac -e $ene -d 2 -c 2
    hadd -f "$ene"GeV_VV.root result_t*

done
