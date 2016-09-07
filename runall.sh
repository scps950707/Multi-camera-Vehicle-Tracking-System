#! /bin/sh
cd build
make
echo 'process:1'
./project -i ~/dev/711.avi -j ~/dev/kymco.avi -o output.avi -n
echo 'process:2'
./project -i ~/dev/7112.avi -j ~/dev/kymco2.avi -o output2.avi -n
echo 'process:3'
./project -i ~/dev/7113.avi -j ~/dev/kymco3.avi -o output3.avi -n
echo 'process:4'
./project -i ~/dev/7114.avi -j ~/dev/kymco4.avi -o output4.avi -n
echo 'process:5'
./project -i ~/dev/7115.avi -j ~/dev/kymco5.avi -o output5.avi -n
echo 'done'
