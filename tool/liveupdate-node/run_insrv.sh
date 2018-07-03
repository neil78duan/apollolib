#!/bin/sh
#
# after server program upload to server , call this unzip and deploy it .
#  create by duan
# 2015-1-15
# usage: run_insrv.sh package_name

#source /home/ec2-user/.bashrc

pack_name=$1
cd /home/liveupdate
if [ -f $pack_name".zip" ];then
	[ -d $pack_name ] && rm -rf $pack_name
	unzip -x $pack_name".zip"
	rm $pack_name".zip"
fi

#[ -d bin ] && rm -rf ./bin
#[ -d data ] && rm -rf ./data
#[ -d cfg ] && rm -rf ./cfg

#cp -r ./$pack_name/bin 	.
#cp -r ./$pack_name/cfg 	.
#cp -r ./$pack_name/data .

[ -d log ] || mkdir log


#./bin/setip.sh ./bin/ndxmlset  192.168.8.55  127.0.0.1 /home/apollo

