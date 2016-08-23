#!/bin/sh
cd build
make || exit
cd ..
./build/PerspectiveTransform kymco.jpg
