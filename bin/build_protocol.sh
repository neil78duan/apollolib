#!/bin/sh

if [ -f ./protoBuilder ]; then
	./protoBuilder -d ../../../code/resource/msg_config -encode utf8
elif [ -f ./protoBuilder_d ]; then
	./protoBuilder_d -d ../../../code/resource/msg_config -encode utf8
else
	echo "need build tool before export protocol"
	exit 1
fi


[ $? == 0 ] || exit 1

cp ./cpp/* ../../../code/src/netMessage
#cp ./csharp/* ../DemoU3d/Assets/netMessage
