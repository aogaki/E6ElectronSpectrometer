#!/bin/bash

echo "/run/beamOn 100000" > tmp.mac


for ene in 1 2 3 4 5 6 7 8 9 10
do
    #./E6ES -v -z -e $ene -m tmp.mac
    #hadd -f ref$ene.root result_t*

    #./E6ES -e $ene -m tmp.mac
    #hadd -f air$ene.root result_t*

    ./E6ES -c 1 -e $ene -m tmp.mac
    hadd -f cor$ene.root result_t*
done
