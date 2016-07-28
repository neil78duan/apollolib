#!/bin/sh

#  genCodeData.sh
#  data_tool
#
#  Created by duanxiuyun on 15-3-6.
#

if [ $# -lt 4 ]; then 
	echo "usage: exp_xls.sh file-list.txt input-excel-path output-text-path [encode-type(gbk|utf8)] [is-auto-svn-up 0|1]"
	exit 1 
fi

workDir=`pwd`

in_filelist=$1
inputPath=$2
outputPath=$3

ENCODE_TYPE="utf8"
[ "x$4" == "x" ] || ENCODE_TYPE=$4

if [ $# -ge 6 ]; then
	if [ "x$5" == "x0" ]; then 
		cd $inputPath 
		svn up 
		cd $workDir
	fi 
fi

[ -d $outputPath ] || mkdir $outputPath

[ -d  $inputPath ] ||  echo "input path not exist" 
[ -d  $inputPath ] || exit ;

#check out svn
#if [ -d  $inputPath ]; then
#    cd $inputPath
#	svn up
#	if [ $? -ne 0 ]; then
#		echo "svn update error !"
#		exit 1
#	fi
#else
#   svn co $src_svn_path    $inputPath
#	if [ $? -ne 0 ]; then
#		echo "svn check out error !"
#		exit 1
#	fi
#fi


#cd $workDir
#svn up
#if [ $? -ne 0 ]; then
#	echo "svn update error !"
#	exit 1
#fi


cat $in_filelist | col -b > ./filelist-unix.txt

#outputPath=$workDir"/"$outputPath
#inputPath=$workDir"/"$inputPath

READEXCL="python ./xls2txt_unix.py"

run_export()
{
	[ "x$1" == "x" ] && return 0
	infile=$1
	outfile=$infile".txt"
	if [ -f $infile ]; then
		echo "[$infile] not exist"
		return 0 ;
	fi

	$READEXCL $inputPath/$infile $ENCODE_TYPE > $outputPath/$outfile
	if [ $? -ne 0 ]; then
		echo "coever $1 error "
		exit 1
	fi
	echo "coever $1 SUCCESS !!! "
}

while read line  
do
	echo "begin export $line"
	if [ "${line:0:1}" != "#" ]; then
		run_export $line
	fi

done < ./filelist-unix.txt


echo "================coever all SUCCESS !!! ========================"
exit 0