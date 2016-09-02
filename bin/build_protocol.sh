#!/bin/sh

./protoBuilder_d -d ../../resource/msg_config -encode utf8

[ $? == 0 ] || exit 1

cp ./cpp/* ../../src/netMessage