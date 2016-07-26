#!/bin/sh

#  publish.sh
#  SheldonServer
#
#  Created by duanxiuyun on 14-8-19.
#

package_name="liveupdate-node"
SheldonPubDir=LiveUpPublish

orgDir=`pwd`
#svn up
#svnInfo=`svn info | grep Revision`
cd ..
workDir=`pwd`

rm -rf $SheldonPubDir/$package_name
cp -r $package_name  $SheldonPubDir/

cd $SheldonPubDir

rm -f $package_name/configuration.json
cp ./configuration-release.json ./$package_name/configuration.json

cd $package_name
rm -rf log1 uploadversion ftp_nodesrv.xcodeproj
rm -f *.sh

# Make version
echo "Author:  $USER " >> VERSION.txt
#echo SVN : $svnInfo >> VERSION.txt
date "+DATE: %Y-%m-%d%nTIME: %H:%M:%S" >> VERSION.txt
cd ..

rm -f $package_name".zip"
zip -r $package_name".zip" $package_name
rm -rf $package_name



update_hostFile()
{
	hostName="$1"
	pem_file="$2"

	echo "begin update $hostName"

	ssh -tt $pem_file $hostName "sudo nohup /etc/init.d/liveupdate stop "
	sleep 1
	echo "stop service success!"

 	scp $pem_file $package_name".zip"  $hostName:/home/liveupdate
 	[ $? -ne 0 ] && return 1
 	echo "Copy $package_name to $hostName success!"

 	ssh -t $pem_file $hostName "/home/liveupdate/run_insrv.sh $package_name"
 	[ $? -ne 0 ] && return 1
 	echo "unzip file success!"

	ssh -tt $pem_file  $hostName "sudo nohup /etc/init.d/liveupdate start "
	sleep 1
	echo "update server program to $hostName SUCCESS!"
}


# update file to aws-np-2
# update_hostFile $ROOT_INTER

update_hostFile $AWS_APO_HOST "-i $AWS_APO_PEM"
cd $orgDir

