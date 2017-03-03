#!/bin/bash


for ene in 0.5 1 3 5
do
    for ((i=1;i<=10;i++))
    do

	echo "/run/beamOn 100000" > tmp.mac
	./E6ES -w -d 3 -c 1 -e $ene -m tmp.mac
	hadd -f case1ene$ene.$i.root result_t*
	
	./E6ES -w -d 3 -e $ene -m tmp.mac
	hadd -f case2ene$ene.$i.root result_t*

	echo "/run/beamOn 1000000" > tmp.mac
	./magnet -v -w -e $ene -m tmp.mac
	hadd -f case3ene$ene.$i.root result_t*
	
	./foil -v -w -e $ene -m tmp.mac
	hadd -f case4ene$ene.$i.root result_t*
	
    done
done
