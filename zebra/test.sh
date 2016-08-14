#!/bin/sh
cd build
make || exit
cd ..
./build/execute 2.jpg 4-2.png
