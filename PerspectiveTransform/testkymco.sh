#!/bin/sh
cd build
make || exit
cd ..
./build/PerspectiveTransform -k kymco.jpg
