#!/bin/bash

echo "/run/beamOn 100000" > tmp.mac

for ((i=1;i<=200;i++))
do
    ./E6ES -p 1 -w -m tmp.mac
    hadd -f all$i.root result_t*

done
