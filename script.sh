#!/bin/bash


for pos in 500 600 700 800 900 1000
do
    echo "/run/beamOn 1000000" > tmp.mac
    for ene in 1 2 3 4 5 6 7 8 9 10
    do
	./E6ES$pos -e $ene -m tmp.mac -d 2
	hadd -f "$ene"GeV$pos.root result_t*
    done

    echo "/run/beamOn 11900" > tmp.mac
    ./E6ES$pos -r -m tmp.mac -d 2
    mv result_t0.root Ref"$ene"GeV$pos.root
done
