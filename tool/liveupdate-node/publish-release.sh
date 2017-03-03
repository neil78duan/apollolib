#!/bin/sh

#  publish.sh
#  SheldonServer
#
#  Created by duanxiuyun on 14-8-19.
#

update_hostFile()
{
	hostName=$2
	pemFile=$1

	echo "begin update $hostName"
	ssh -t -i $pemFile $hostName "sudo nohup /etc/init.d/sheldond stop "
	scp -i $pemFile node-srv.zip $hostName:/home/Sheldon
	ssh -t -i $pemFile $hostName "/home/Sheldon/tool/auto_sheldon"
	ssh -t -i $pemFile $hostName "sudo nohup /etc/init.d/sheldond start "

	echo "update $hostName SUCCESS"
}

update_hostFileNoRestart()
{
	hostName=$2
	pemFile=$1

	echo "begin update $hostName"
	scp -i $pemFile node-srv.zip $hostName:/home/Sheldon
	ssh -t -i $pemFile $hostName "/home/Sheldon/tool/auto_sheldon"

	echo "update $hostName SUCCESS"
}



orgDir=`pwd`
svn up

svnInfo=`svn info | grep Revision`
cd ..
workDir=`pwd`

cp -r node-srv $SheldonPubDir

cd $SheldonPubDir
cd node-srv
rm -f configuration.json

# Make version
echo "Author:  $USER " >> VERSION.txt
echo SVN : $svnInfo >> VERSION.txt
date "+DATE: %Y-%m-%d%nTIME: %H:%M:%S" >> VERSION.txt


cp ../configuration-release.json  ./configuration.json
cd ..


rm -f node-srv.zip

zip -r node-srv.zip node-srv

rm -rf node-srv

# update gm-tool live server
update_hostFile $YESCHEF_PEM $YESCHEF_HOST

#update_hostFile $YESCHEF_PEM $YESCHEF_GAME1

#update_hostFile $YESCHEF_PEM $YESCHEF_GAME2

#update backup server
#update_hostFileNoRestart $YESCHEF_PEM $YESCHEF_DIR_BACKUP

#update backup but not restart
#scp -i $YESCHEF_PEM node-srv.zip $YESCHEF_DIR_BACKUP
#ssh -t -i $YESCHEF_PEM $YESCHEF_BACKUP "/home/Sheldon/tool/auto_sheldon"


cd $orgDir