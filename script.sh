#!/bin/bash


echo "/run/beamOn 10000" > tmp.mac

for ((i=0;i<10;i++))
    do
	for hole in 1 2 3 4 5 6 7 8
	do
	    for ene in 1 2 3 4 5 6 7 8 9 10
	    do
		./hole"$hole"mm -c 1 -d 3 -e $ene -m tmp.mac
		hadd -f hole"$hole"Ene$ene.$i.root result_t*
	    done
	done
done
