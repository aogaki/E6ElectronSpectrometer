#!/bin/bash

echo "/run/beamOn 1000000" > tmp.mac

./E6ES -m tmp.mac -d 2 -c 1
hadd -f collimator.root result_t*
