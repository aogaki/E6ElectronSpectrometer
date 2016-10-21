#!/bin/bash

nEvents=1000000

for ene in 1 2 8 9
do
    echo "/ES/Geometry/WindowMaterial G4_Galactic" > tmp.mac
    echo "/run/beamOn $nEvents" >> tmp.mac
    
    ./E6ES -m tmp.mac -e $ene
    hadd -f noWindow"$ene"GeV.root result_t*
    
    ./E6ES -m tmp.mac -e $ene -z
    hadd -f noWindow"$ene"GeVNoAng.root result_t*
    
    rm -f tmp.mac
done

for ene in 1 2 8 9
do
    echo "/run/beamOn $nEvents" > tmp.mac
    
    ./E6ES -m tmp.mac -e $ene
    hadd -f polycarbonate"$ene"GeV.root result_t*

    ./E6ES -m tmp.mac -e $ene -z
    hadd -f polycarbonate"$ene"GeVNoAng.root result_t*
    
    rm -f tmp.mac
done

rm -f result*
