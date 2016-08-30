#!/bin/sh
cd build
make || exit
cd ..
./build/PerspectiveTransform -s 7-11.jpg
