#!/bin/sh

#  get_svn_ver.sh
#  
#
#  Created by duanxiuyun on 2017/8/21.
#

workDir=`pwd`

dataPath=$1

cd $dataPath
# svnInfo=`svn info | grep Revision`
svnInfo="unknown svn "
datasvnInfo="unknown svn"
# datasvnInfo=`svn info $GAME_DATA_SVN_URL | grep Revision`
platInfo=`uname -rms`


echo "id	svn	desc"
echo "id	svn	desc"
echo "1	$svnInfo	create by $user on $HOSTNAME,build time: `date "+DATE: %Y-%m-%d% TIME: %H:%M:%S"`,$svnInfo platform:$platInfo"
