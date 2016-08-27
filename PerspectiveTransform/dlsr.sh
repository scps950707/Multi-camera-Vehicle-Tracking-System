#!/bin/sh
cd build
make || exit
cd ..
./build/PerspectiveTransform DSLR.JPG
