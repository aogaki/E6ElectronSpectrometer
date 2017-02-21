#!/bin/bash

echo "/run/beamOn 1000000" > tmp.mac


for ene in 0.05 0.10 0.20 0.30 0.40 0.50 0.60 0.70 0.80 0.90 1.00
do
    ./E6ES -z -e $ene -m tmp.mac
    hadd -f ene$ene.root result_t*
done
