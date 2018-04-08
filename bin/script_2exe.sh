#!/bin/sh

workDir=`pwd`

cd ../xcode

#xcodebuild clean -target apoScriptTool

xcodebuild -workspace apoLibs.xcworkspace -scheme apoScriptTool -configuration Debug
if [ $? -ne 0 ]; then
	echo "make cpp to exe-file error"
	exit 1
fi

cd $workDir
#cp ./cppTemplate $1/$2
echo "compile success outputfile:$1/$2"

