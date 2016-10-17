#!/bin/bash

for file in `ls *.root`
do
    root -l -q "MakeHists.cpp+O(\"$file\")"
    mv ene.pdf $file.ene.pdf
    mv theta.pdf $file.theta.pdf
    mv pos.pdf $file.pos.pdf

    mv ene.txt $file.ene.txt
    #mv theta.txt $file.theta.txt
    mv pos.txt $file.pos.txt
done
