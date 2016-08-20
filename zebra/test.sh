#!/bin/sh
cd build
make || exit
cd ..
./build/execute 7-11.jpg kymco.jpg
