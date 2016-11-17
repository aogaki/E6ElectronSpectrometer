#!/bin/bash


echo "/run/beamOn 1000000" > tmp.mac

    for det in 1 2	       
    do
	./E6ES -m tmp.mac -d $det
	hadd -f NamBeam$det.root result_t*
    done
