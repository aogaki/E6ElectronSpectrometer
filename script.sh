#!/bin/bash

nEvents=1000000

echo "/ES/Geometry/WindowMaterial G4_Galactic" > tmp.mac
echo "/run/beamOn $nEvents" >> tmp.mac

./E6ES -m tmp.mac
hadd -f noWindow.root result_t*

rm -f tmp.mac

for mat in Acrylic G4_POLYCARBONATE
do
    for t in 3 5 10
    do
	echo "/ES/Geometry/WindowMaterial $mat" > tmp.mac
	echo "/ES/Geometry/WindowThickness $t" >> tmp.mac
	echo "/run/beamOn $nEvents" >> tmp.mac
	./E6ES -m tmp.mac
	hadd -f "$mat"T$t.root result_t*
    done
done

rm -f result*
