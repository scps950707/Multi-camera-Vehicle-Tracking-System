#!/bin/sh
cd build
make || exit
cd ..
./build/testptrans -k kymco.jpg
