#!/bin/bash


echo "/run/beamOn 1000000" > tmp.mac

for ene in 1 2 3 4 5 6 7 8 9 10
do
    for det in 1 2	       
    do
	./E6ES -m tmp.mac -e $ene -d $det
	hadd -f "$ene"GeV"$det".root result_t*
    done
done
