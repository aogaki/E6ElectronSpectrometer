#!/bin/bash

echo "/run/beamOn 100000" > tmp.mac
#for hole in 1 2 3 4 5 6 7 8
for hole in 4
do
#    for ene in 1 2 3 4 5 6 7 8 9 10
    for ene in 10
    do
	./E6ES$hole -e $ene -m tmp.mac -c 1 -d 2
	hadd -f "$ene"GeV$hole.root result_t*
    done
done

