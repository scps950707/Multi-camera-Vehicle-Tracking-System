#! /bin/sh
EXE=./project
cd build
make
START=$(date +%s)
echo 'process:1'
$EXE -i ~/dev/711.avi -j ~/dev/kymco.avi -o output.avi -n
END=$(date +%s)
echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}'

echo 'process:2'
START=$(date +%s)
$EXE -i ~/dev/7112.avi -j ~/dev/kymco2.avi -o output2.avi -n
END=$(date +%s)
echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}'

echo 'process:3'
START=$(date +%s)
$EXE -i ~/dev/7113.avi -j ~/dev/kymco3.avi -o output3.avi -n
END=$(date +%s)
echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}'

echo 'process:4'
START=$(date +%s)
$EXE -i ~/dev/7114.avi -j ~/dev/kymco4.avi -o output4.avi -n
END=$(date +%s)
echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}'

echo 'process:5'
START=$(date +%s)
$EXE -i ~/dev/7115.avi -j ~/dev/kymco5.avi -o output5.avi -n
END=$(date +%s)
echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}'

echo 'done'
