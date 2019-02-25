#!/bin/sh


#  genCodeData.sh
#  data_tool
#
#  Created by duanxiuyun on 15-3-6.
#

if [ $# -lt 4 ]; then
	echo "usage: exp_xls.sh file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] "
	exit 1
fi

workDir=`pwd`

in_filelist=$1
inputPath=$2
outputPath=$3

ENCODE_TYPE="utf8"
[ "x$4" == "x" ] || ENCODE_TYPE=$4

[ -d $outputPath ] || mkdir $outputPath

[ -d  $inputPath ] ||  echo "input path not exist"
[ -d  $inputPath ] || exit ;

chmod u+x ./get_svn_ver.sh
./get_svn_ver.sh $inputPath > $outputPath\version.txt

READEXCL="python ./XLStoTxt2.py"

while read line
do
	echo "begin export $line"
	if [ "${line:0:1}" != "#" ]; then		
		$READEXCL $inputPath $line  $outputPath $ENCODE_TYPE
		if [ $? -ne 0 ]; then
		echo "coever $1ine error "
		exit 1
	fi
	fi

done < $in_filelist


echo "================coever all SUCCESS !!! ========================"
exit 0
