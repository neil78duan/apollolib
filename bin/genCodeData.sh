#!/bin/sh

#  genCodeData.sh
#  data_tool
#
#  Created by duanxiuyun on 15-3-6.
#
workDir=`pwd`

outputPath="outputData"
inputPath="game_data"
src_svn_path="svn://192.168.8.55/design/game_data"

[ -d $outputPath ] || mkdir $outputPath

[ -d  $inputPath ] ||  svn co $src_svn_path    $inputPath
svn up $inputPath
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


cd $workDir
#svn up
#if [ $? -ne 0 ]; then
#	echo "svn update error !"
#	exit 1
#fi

OSkernel=`uname -s | tr '[A-Z]' '[a-z]'`

cat filelist.txt | col -b > filelist-unix.txt

#if [ "$OSkernel" == "linux" ]; then
#	sed -i 's/\r$//' filelist.txt
#else
#	echo "convert text file"
#	cat filelist.txt | tr -d "\n" > filelist_mac.txt
#	mv filelist_mac.txt filelist.txt
#fi





outputPath=$workDir"/"$outputPath
inputPath=$workDir"/"$inputPath


READEXCL="python ./xls2txt_unix.py"

ENCODE_TYPE="utf8"
[ "x$2" == "x" ] || ENCODE_TYPE=$2


run_export()
{
	[ "x$1" == "x" ] && return 0
	infile=$1
	outfile=$infile".txt"
	#outfile=${infile/".xlsx"/".txt"}
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

done < filelist-unix.txt


echo "================coever all SUCCESS !!! ========================"
exit 0