#!/bin/bash

echo "/run/beamOn 100000" > tmp.mac

for front in 0 3
do
    for mirror in 0 1
    do
	for ene in 0.5 1 2 3 4 5
	do
	    ./E6ES -c 1 -e $ene -p $mirror -d $front -m tmp.mac
	    hadd -f ene"$ene"mirror"$mirror"det"$front"col1.root result_t*

	    ./E6ES -c 1 -w -e $ene -p $mirror -d $front -m tmp.mac
	    hadd -f wide1ene"$ene"mirror"$mirror"det"$front"col1.root result_t*
	done
    done
done
