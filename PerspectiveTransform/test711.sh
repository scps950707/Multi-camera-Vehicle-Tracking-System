#!/bin/sh
cd build
make || exit
cd ..
./build/PerspectiveTransform 7-11.jpg
