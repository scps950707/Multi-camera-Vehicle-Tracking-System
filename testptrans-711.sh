#!/bin/sh
cd build
make || exit
cd ..
./build/testptrans -s 7-11.jpg
