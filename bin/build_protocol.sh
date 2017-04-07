#!/bin/sh

if [ -f ./protoBuilder ]; then
	./protoBuilder -d ../../resource/msg_config -encode utf8
elif [ -f ./protoBuilder_d ]; then
	./protoBuilder_d -d ../../resource/msg_config -encode utf8
else
	echo "need build tool before export protocol"
	exit 1
fi


[ $? == 0 ] || exit 1

cp ./cpp/* ../../src/netMessage
