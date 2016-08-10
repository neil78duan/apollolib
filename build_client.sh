#!/bin/sh
#
# this is auto build xcode project 
#
# call xcodebuild directly not makefile 
#
# 2015-1-22
#

workDir=`pwd`

#build android
cd proj.androin
ndk-build clean
ndk-build
[ $? == 0 ] || exit 1

cd $workDir

cd xcode/apollo_client
xcodebuild clean -target apollo_client_ios
xcodebuild clean -target apollo_client_mac

xcodebuild -project apollo_client.xcodeproj -target apollo_client_ios
[ $? == 0 ] || exit 1


xcodebuild -project apollo_client.xcodeproj -target apollo_client_mac
[ $? == 0 ] || exit 1

cd $workDir

