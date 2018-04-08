#!/bin/sh

workDir=`pwd`

cd ../cfg/cpptmpl
make 
if [ $? -ne 0 ]; then
	echo "make cpp to exe-file error"
	exit 1
fi

cd $workDir
cp ./cppTemplate $1/$2
echo "compile success outputfile:$1/$2"

