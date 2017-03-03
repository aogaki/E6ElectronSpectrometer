#!/bin/bash


for ((i=1;i<=10;i++))
do
    echo "/run/beamOn 1000000" > tmp.mac
    ./E6ES -m tmp.mac
    hadd -f front$i.root result_t*
    
    ./E6ES -d 3 -p 1 -m tmp.mac
    hadd -f mirror$i.root result_t*
    
    ./E6ES -d 3 -m tmp.mac
    hadd -f normal$i.root result_t*
    
    echo "/run/beamOn 100000" > tmp.mac
    ./E6ES -w -d 3 -m tmp.mac
    hadd -f wide$i.root result_t*
    
    ./E6ES -c 1 -p 1 -w -m tmp.mac
    hadd -f colli$i.root result_t*
    
    ./E6ES -p 1 -w -m tmp.mac
    hadd -f all$i.root result_t*

done
