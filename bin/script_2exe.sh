#!/bin/sh

workDir=`pwd`

echo "ndhome=$NDHOME"
echo "apollolib=$LIBAPOLLO"

#cd ../xcode
#xcodebuild clean -target apoScriptTool
#xcodebuild -workspace apoLibs.xcworkspace -scheme apoScriptTool -configuration Debug

cd ../cfg/cpptmpl/
if [ "$3" == "debug" ]; then 
	make DEBUG="y"
else 
	make DEBUG="n"
fi

if [ $? -ne 0 ]; then
	echo "make cpp to exe-file error"
	exit 1
fi

cd $workDir
cp ./cppTemplate $1/$2
echo "compile success outputfile:$1/$2"

