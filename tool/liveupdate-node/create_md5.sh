#!/bin/sh

#  config.sh
#  config for ndsdk
#
#  Created by duan .
#

orgDir=`pwd`

if [ $# -lt 1 ] ; then
	read -p "Input version-id :"  verID
else
	verID=$1
fi

echo "version-id = $verID "

cd $verID

createTm=`date "+DATE: %Y-%m-%d TIME: %H:%M:%S"`

#varDataName="data.zip"
#varFestivalName=`ls | grep festival`

logFileName="MD5_info.txt"

[ -f version.txt ] || echo $verID > version.txt


#echo "{" > $logFileName

echo "Author=$USER," > $logFileName

for x in $(ls)
	do
	if [ -f "$x" ];then
		[ "$x" == "$logFileName" ] && continue
		md5 -q $x > datamd5.tmp
		read -p " :"  dataMD5 < datamd5.tmp
		rm -f datamd5.tmp
		echo "$x=$dataMD5," >> $logFileName
	fi
done

echo "DateTime=$createTm" >>  $logFileName

#echo "}" >>  $logFileName

echo "md5 success"

cd $orgDir
